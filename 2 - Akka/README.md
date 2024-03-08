# Evaluation lab - Akka

## Group number: 19

## Group members

- Student 1 Alessandro Fornara
- Student 2 Matteo Figini
- Student 3 Fabrizio Monti

## Description of message flows
At the beginning the main process creates the dispatcher and sends its reference
to each TemperatureSensor through ConfigMsg. When the dispatcher is created, it also creates
the SensorProcessorActors and becomes their supervisor.

By default, the dispatcher start with the Load Balancer configuration; this setting
can be changed to Round Robin and viceversa through the DispatchLogicMsg.

The main process tells to each TemperatureSensor through a GenerateMsg to generate
a new sample; This sample is sent to the Dispatcher by the TemperatureSensor
through TemperatureMsg. After that, the Dispatcher delivers the message to a 
SensorProcessor according to the logic adopted at that time. Finally,
The SensorProcessor updates the average temperature received if it's non negative.
If the temperature is negative, the SensorProcessor throws a new Exception that 
is handled by its supervisor (Dispatcher) which uses a resume strategy to keep the 
data stored till that moment.

