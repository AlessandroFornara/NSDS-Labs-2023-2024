import akka.actor.*;
import akka.japi.pf.DeciderBuilder;

import java.time.Duration;
import java.util.ArrayList;
import java.util.HashMap;

public class DispatcherActor extends AbstractActorWithStash {

	private final static int NO_PROCESSORS = 2;
	// A list containing the references of the processors
	private ArrayList<ActorRef> processors = new ArrayList<>();
	//Key: TemperatureSensorActor
	//Value: SensorProcessorActor
	//This map keeps track of the sensors assigned to each processor
	private HashMap<ActorRef, ActorRef> processorsMapping = new HashMap<>();

	// This index represents the processor that should be assigned a newly introduced
	// temperature sensor with load balancing
	private int idxLastProcessorLB = 0;

	// This index represent the sensor that should process the next data with round robin
	private int idxLastProcessorRR = 0;
	private static SupervisorStrategy strategy =
			new OneForOneStrategy(
					1,
					Duration.ofMinutes(1),
					DeciderBuilder.match(Exception.class, e -> SupervisorStrategy.resume()).build()
			);

	@Override
	public SupervisorStrategy supervisorStrategy() {
		return strategy;
	}

	public DispatcherActor() {
		for(int i = 0; i<NO_PROCESSORS; i++){
			processors.add(getContext().actorOf(SensorProcessorActor.props()));
		}
	}


	@Override
	public AbstractActor.Receive createReceive() {
		return createReceiveLoadBalancer();
	}

	public AbstractActor.Receive createReceiveLoadBalancer() {
		return receiveBuilder().
				match(DispatchLogicMsg.class, this::changeStrategy).
				match(TemperatureMsg.class, this::dispatchDataLoadBalancer)
				.build();
	}

	private void changeStrategy(DispatchLogicMsg msg){
		if(msg.getLogic() == 0){
			System.out.println("Changing to Round Robin");
			getContext().become(createReceiveRoundRobin());
		}
		else{
			System.out.println("Changing to Load Balancer");
			getContext().become(createReceiveLoadBalancer());
		}
	}

	public AbstractActor.Receive createReceiveRoundRobin() {
		return  receiveBuilder()
				.match(DispatchLogicMsg.class, this::changeStrategy)
				.match(TemperatureMsg.class, this::dispatchDataRoundRobin)
				.build();
	}

	private void dispatchDataLoadBalancer(TemperatureMsg msg) {
		if(!processorsMapping.containsKey(msg.getSender())){
			processorsMapping.put(msg.getSender(), processors.get(idxLastProcessorLB));
			idxLastProcessorLB = (idxLastProcessorLB + 1) % NO_PROCESSORS;
		}
		System.out.println("DISPATCHER W/LOAD BALANCING: temp to " + processorsMapping.get(msg.getSender()));
		processorsMapping.get(msg.getSender()).tell(msg, self());
	}

	private void dispatchDataRoundRobin(TemperatureMsg msg) {
		processors.get(idxLastProcessorRR).tell(msg, self());
		System.out.println("DISPATCHER W/ROUND ROBIN: temp to " + idxLastProcessorRR);
		idxLastProcessorRR = (idxLastProcessorRR + 1) % NO_PROCESSORS;
	}

	static Props props() {
		return Props.create(DispatcherActor.class);
	}
}
