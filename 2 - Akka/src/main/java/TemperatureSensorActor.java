import java.util.concurrent.ThreadLocalRandom;

import akka.actor.AbstractActor;
import akka.actor.ActorRef;
import akka.actor.Props;

public class TemperatureSensorActor extends AbstractActor {

	private ActorRef dispatcher;
	private final static int MIN_TEMP = 0;
	private final static int MAX_TEMP = 50;

	@Override
	public AbstractActor.Receive createReceive() {
		return receiveBuilder()
				.match(GenerateMsg.class, this::onGenerate).
				match(ConfigMsg.class, this::onConfigMsg)
				.build();
	}

	private void onConfigMsg(ConfigMsg msg) {
		System.out.println("TEMPERATURE SENSOR: Setting dispatcher");
		this.dispatcher = msg.getDispatcher();
	}


	private void onGenerate(GenerateMsg msg) {
		System.out.println("TEMPERATURE SENSOR: Sensing temperature!");
		int temp = ThreadLocalRandom.current().nextInt(MIN_TEMP, MAX_TEMP + 1);
		dispatcher.tell(new TemperatureMsg(temp,self()), self());
	}

	static Props props() {
		return Props.create(TemperatureSensorActor.class);
	}

}
