# Evaluation lab - Contiki-NG

## Group number: 19

## Group members

- Matteo Figini
- Alessandro Fornara
- Fabrizio Monti

## Solution description
Upon receiving a message the server checks the sender ip address. If the sender was already registered (its ip is saved inside the array "clients") the server processes the temperature. Otherwise the server checks how many clients are registered: if there are less than MAX_RECEIVERS the new sender is registered, otherwise the packet is discarded.

The clients generate a new temperature value each minute and tries to send it to the server. If there is a connection error the client adds the value to a queue, waiting for the connection to be established again. Once the connection is working again the clients sends the average of the saved values and the newly read temperature value.
