import akka.actor.AbstractActor;
import akka.actor.Props;

public class SensorProcessorActor extends AbstractActor {

	private double currentAverage = 0;
	private int numTempReceived = 0;

	@Override
	public Receive createReceive() {
		return receiveBuilder().match(TemperatureMsg.class, this::gotData).build();
	}

	private void gotData(TemperatureMsg msg) throws Exception {

		System.out.println("SENSOR PROCESSOR " + self() + ": Got data from " + msg.getSender() + " with value " + msg.getTemperature());
		if(msg.getTemperature() < 0){
			System.out.println("Processor failing");
			throw new Exception("Negative temp received");
		}
		else{
			numTempReceived++;
			currentAverage = (currentAverage*(numTempReceived-1) + msg.getTemperature())/numTempReceived;
		}
		System.out.println("SENSOR PROCESSOR " + self() + ": Current avg is " + currentAverage);
	}

	static Props props() {
		return Props.create(SensorProcessorActor.class);
	}

	public SensorProcessorActor() {
	}
}
