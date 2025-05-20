# CN_CA3


## Phase 5:

**PLEASE PAY ATTENTION**
- Copy `Taasiaan.mp3` to ./assets/ (removed to reduce upload size)
change these addresses to proper addresses:
- applicationcontext.cpp:4
- datagenerator.cpp:53

**READ THESE**
- TCPNewReno
- networknode::PC::processTcpReceiver
- networknode::PC::processTcpSender
- DataGenerator::generateBuffers
- DataGenerator::splitFileToPackets

**Other Things To Consider...**
- I used my own codebase for this phase. 
- Program has been tested and works fine.
- Output will be saved in ./assets/output.mp3
- Wait until end of simulation - about 1 minute (state change to 4 will be written in cmd, write `stats` to ensure packet loss = 0)
- A `printProgress` is written to show real-time packets gathering of receiver (PC 24). 
- `Local8Bit` is used to encode/decode.


### DataGenerator:

- `DataGenerator` read an mp3 file and distribute it between 14 PCs (from 25 to 38).
- Each PC receives a buffer which contains 1630 (or 1631) packets of 1'024 KB data.
- Each Packet has a `sequenceNumber` and a `tcpDataSeq` where sequenceNumber defines order of packets in a buffer and tcpDataSeq defines order of data.


### TCPNewReno:

- Contains two class `TCPNewRenoReceiver` and `TCPNewRenoSender`.
- TCPNewRenoReceiver has a `expectedSeqNum` defines expected sequenceNumber for next packet.
- TCPNewRenoReceiver also has a `lastAck` which is a cummaltive acknowledgment for received packets.

- TCPNewRenoSender contains:
    - `cwnd`: Congestion Window
    - `ssthresh`: Slow Start Threshold
    - `nextSeqNum`: Next packet to send
    - `lastAcked`: Last acknowledged packet
    - `duplicateAcks`: Counter for duplicate ACKs
    - `inRecovery`: Whether we are in recovery or not (for newReno)
    - `ackeds`: Acked packets, used for SACK
    - `head`: next packet to send

- I also implement SACK for more efficiancy.

### PC:

`processTcpReceiver` Function
- Handles incoming TCP packets at the receiver side.
- Steps:
  - Stores the payload of the packet in a buffer using the sequence number as the key.
  - Checks if the received packet's sequence number matches the expected sequence number (`expectedSeqNum`).
    - If it matches, updates the `lastAck` and increments `expectedSeqNum`.
  - Generates an acknowledgment (ACK) packet with the `lastAck` value and enqueues it for sending.
  - Calls `printProgress` to display the current state of the receiver buffer and TCP info.

`processTcpSender` Function
- Handles incoming ACK packets at the sender side and implements TCP NewReno congestion control.
- Steps:
  - Extracts the ACK number from the packet.
  - Duplicate ACKs Handling:
    - If the ACK number is the same as the last acknowledged number (`lastAcked`), increments the `duplicateAcks` counter.
    - If 3 duplicate ACKs are received:
      - Triggers Fast Retransmit:
        - Reduces the congestion window (`cwnd`) to half (`ssthresh`).
        - Enters Fast Recovery mode.
    - If more than 3 duplicate ACKs are received in Fast Recovery, increments `cwnd` by 1 (window inflation).
  - New ACK Handling:
    - If the ACK number is new:
      - Updates `lastAcked` and resets `duplicateAcks`.
      - If in Fast Recovery:
        - Exits Fast Recovery and sets `cwnd` to `ssthresh`.
      - If not in Fast Recovery:
        - Slow Start: Doubles `cwnd` if it’s below `ssthresh`.
        - Congestion Avoidance: Increments `cwnd` by 1 if it’s above or equal to `ssthresh`.


Features of TCP NewReno in the Code
- Fast Retransmit: Triggered by 3 duplicate ACKs.
- Fast Recovery: Allows the sender to continue transmitting new packets during recovery.
- Congestion Window Management:
  - Slow Start: Exponential growth of `cwnd` until `ssthresh` is reached.
  - Congestion Avoidance: Linear growth of `cwnd` after `ssthresh`.
- Duplicate ACK Handling: Detects packet loss without waiting for a timeout.


### TCP NewReno, TCP Vegas and TCP Veno comparison

TCP Veno, TCP NewReno, and TCP Vegas are different congestion control algorithms used in TCP (Transmission Control Protocol) to manage data flow and avoid network congestion. Each has its own approach to detecting and responding to congestion. Here's a comparison of the three:

`TCP NewReno`
- Congestion Detection: Relies on packet loss as an indication of congestion.
- Recovery Mechanism: Improves upon TCP Reno by handling multiple packet losses within a single window of data more efficiently. It uses Fast Recovery and can recover from multiple losses without requiring a timeout for each loss.
- AIMD (Additive Increase Multiplicative Decrease): Uses the traditional AIMD approach for congestion window adjustment.
- Use Case: Widely used in many implementations due to its robustness and simplicity.

`TCP Vegas`
- Congestion Detection: Uses changes in the round-trip time (RTT) of packets to detect congestion before packet loss occurs.
- Recovery Mechanism: Adjusts the sending rate based on the difference between the expected and actual throughput, aiming to keep a small, stable number of packets buffered in the network.
- Rate Adjustment: Increases or decreases the window size based on the calculated difference in throughput, which is a form of proactive congestion avoidance.
- Use Case: More efficient in environments where packet loss is not the primary indicator of congestion, but it is less commonly deployed due to its complexity and the challenge of fair co-existence with loss-based algorithms.

`TCP Veno`
- Congestion Detection: Combines the approaches of Reno and Vegas by using both packet loss and RTT measurements to detect congestion.
- Recovery Mechanism: Similar to NewReno, it uses Fast Recovery, but it also takes into account the RTT to distinguish between random packet loss and congestion-induced loss.
- Rate Adjustment: Adjusts the congestion window based on the estimated number of backlogged packets in the network buffers, which is inferred from RTT measurements.
- Use Case: Designed to perform well in both wired and wireless networks, where non-congestion-related packet loss is more common.
