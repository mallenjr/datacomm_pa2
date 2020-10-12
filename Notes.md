# What is GBN

## From the Lecture

### Building a Reliable Protocol

One of the main ingerdients for a reliable protocol is the sequence number. We add this ingredient to the SenderReceiver Protocol that we have been building in class. This protocol is a `stop and wait protocol`. The protocol utilization is low because we have to wait for and process ACKs in every round trip. 

### Pipeline Protocols

**pipelineing:** sender allows multiple, "in flight", yet-to-be-acknowledged packets
- range of sequence number must be increased
- buffering at sender and/or receiver
- two generic forms of such protocols: `Go-Back-N, Selective Repeat`

### Onto Go-Back-N

- We use a sliding window to store state on a group of packets that are either outstanding, waiting to be sent, or can be sent.
- On the receiver side, we use a sliding window to track packets that have been accepted or that the reciver is waiting to accept.

#### Metrics

- Bandwidth-Delay Product = (channel bandwith in bits/sec) * (RTT/2)

  - RTT is in seconds
  - whatever we compute here will be expressed as number of packets
- Set window w = 2*BD + 1; maximum number of packets we can have outstanding on the channel and have received no feedback from.