# Evaluation lab - Node-RED

## Group number: XX

## Group members

- Alessandro Fornara
- Matteo Figini
- Fabrizio Monti

## Description of message flows

In our flow each message that is received by the "Telegram receiver" node gets forwarded to the "handle request" node. This node inspects the request and save the type, the time and the city of the request inside the flow. Then it forwards the message to the openweather nodes.

The openweather nodes request data for the next 5 days, divided in 3 hours slots.

The data generated goes then to the "create answer" node that, as the name suggests, creates the answer to be sent to the telegram user. This node checks if the data has changed since the last equal request and computes the answer, which is forwarded to the telegram sender.

The are 3 different nodes, "timestamp", "logger" and "write log" that handles the log. Every 1 minute the "logger" node saves the number of requests received and saves it to a file.

Possible messages that generate the responses:
What tomorrow's forecast in Milan?
What tomorrow's forecast in Rome?
What tomorrow's expected wind in Milan?
What tomorrow's expected wind in Rome?
What's the expected forecast in two days in Milan?
What's the expected forecast in two days in Rome?
What's the expected wind in two days in Milan?
What's the expected wind in two days in Rome?

## Extensions 

## Bot URL 
t.me/group19_eval_bot

Name of the bot: group19_eval_bot
Username: group19_eval_bot
API Key: 6723207823:AAFR78QFRbe2nT0YLZ0cy9UCYTPi2x3PJmw

OpenWeatherMap API Key: 413c4fbcd0bb5e6578a3e6ae94ce99b4