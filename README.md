# DMPTCP

## Compilation

```make server``` for lauching the dmptcp server. It must be run in each computer which want to use the protocol
```make client``` for lauching a dmptcp client over multiple servers in order to test the protocol sending/receiving routines

## For middleboxes

To avoid middleboxes difficulties that concern classic TCP packet modifications, we have to install/launch also the driver (Server_listening) in the middleboxes.
