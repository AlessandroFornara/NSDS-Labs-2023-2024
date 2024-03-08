# Evaluation lab - Apache Kafka

## Group number: 19

## Group members

- Matteo Figini
- Fabrizio Monti
- Alessandro Fornara

## Exercise 1

- Number of partitions allowed for inputTopic (min, max): (1, n), with n >= 1.

- Number of consumers allowed (min, max): (1, m), with m <= n.
    - Consumer 1: "groupA"
    - Consumer 2: "groupA"
    - ...
    - Consumer n: "groupA"

Each consumer will print the received messages which have a value greater than the threshold. 

## Exercise 2

- Number of partitions allowed for inputTopic (min, max): (1, n), with n >= 1.
- Number of consumers allowed (min, max): (1, 1)
    - Consumer 1: "groupB"

If there is more than one consumer, each consumer counts "for its own"; that means only for the received messages for 
that consumer and not all generated messages by the producer.
We assumed that keys are between 0 and 999; moreover, the first exercise was running on "groupA" and the second exercise 
on "groupB".