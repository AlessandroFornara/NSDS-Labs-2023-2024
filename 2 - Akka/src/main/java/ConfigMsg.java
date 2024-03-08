import akka.actor.ActorRef;

public class ConfigMsg {

    private final ActorRef dispatcher;
    public ConfigMsg(ActorRef dispatcher){
        this.dispatcher = dispatcher;
    }

    public ActorRef getDispatcher() {
        return dispatcher;
    }
}
