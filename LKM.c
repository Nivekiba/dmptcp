/*****************************************************
 * This code was compiled and tested on Ubuntu 18.04.1
 * with kernel version 4.15.0
 *****************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>


#define NIPQUAD(addr) \
    ((unsigned char *)&addr)[0], \
    ((unsigned char *)&addr)[1], \
    ((unsigned char *)&addr)[2], \
    ((unsigned char *)&addr)[3]


static struct nf_hook_ops *nfho = NULL;

static unsigned int hfunc(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct iphdr *iph;
	struct udphdr *udph;
	struct tcphdr *tcph;

	if (!skb)
		return NF_ACCEPT;

	iph = ip_hdr(skb);
	if (iph->protocol == IPPROTO_UDP) {
		udph = udp_hdr(skb);
		if (ntohs(udph->dest) == 53) {
			return NF_ACCEPT;
		}
	}
	else if (iph->protocol == IPPROTO_TCP) {
		tcph = tcp_hdr(skb);
		//printk("source address: (%pI4)\ndestination adress: (%pI4)\nDestination port: %d", &(iph->saddr), &(iph->daddr), ntohs(tcph->dest));
		return NF_DROP;
	}

	if(iph->protocol==IPPROTO_ICMP) {
        printk(KERN_INFO "=== BEGIN ICMP ===\n");
        printk(KERN_INFO "IP header: original source: %d.%d.%d.%d\n", NIPQUAD(iph->saddr));
        printk(KERN_INFO "IP header: original destin: %d.%d.%d.%d\n", NIPQUAD(iph->daddr));
        //iph->daddr = iph->daddr ^ 0x10000000;
		((unsigned char*)&(iph->daddr))[2] = 8;
        printk(KERN_INFO "IP header: modified source: %d.%d.%d.%d\n", NIPQUAD(iph->saddr));
        printk(KERN_INFO "IP header: modified destin: %d.%d.%d.%d\n", NIPQUAD(iph->daddr));
        printk(KERN_INFO "=== END ICMP ===\n");

		return NF_ACCEPT;

    }
	
	return NF_ACCEPT;
}

static int __init LKM_init(void)
{
	nfho = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
	
	printk(KERN_INFO "Initializing module !!");
	/* Initialize netfilter hook */
	nfho->hook 	= (nf_hookfn*)hfunc;		/* hook function */
	//nfho->hooknum 	= NF_INET_POST_ROUTING;		/* sended packets */
	nfho->hooknum 	= NF_INET_PRE_ROUTING;		/* received packets */
	nfho->pf 	= PF_INET;			/* IPv4 */
	nfho->priority 	= NF_IP_PRI_FIRST;		/* max hook priority */
	
	nf_register_net_hook(&init_net, nfho);

	return 0;
}

static void __exit LKM_exit(void)
{
	printk(KERN_INFO "Uninitializing module !!");
	nf_unregister_net_hook(&init_net, nfho);
	kfree(nfho);
}

MODULE_LICENSE ("GPL v2");
module_init(LKM_init);
module_exit(LKM_exit);
