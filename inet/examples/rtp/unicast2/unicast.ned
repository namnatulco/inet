package inet.examples.rtp.unicast2;

import inet.networklayer.configurator.ipv4.IPv4NetworkConfigurator;
import inet.node.rtp.RTPHost;


network RTPUnicast2
{
    types:
        channel ethernet extends ned.DatarateChannel
        {
            parameters:
                delay = normal(0.00015s, 0.00005s);
                datarate = 10Mbps;
                ber = 0.00001;
        }

    submodules:
        sender: RTPHost {
            parameters:
                forwarding = false;
                profileName = "inet.transportlayer.rtp.RTPAVProfile";
                destinationAddress = "receiver";
                portNumber = 5004;
                bandwidth = 8000;
                fileName = "../data/moving.mpg.gdf";
                payloadType = 32;
                @display("p=185,128");
        }
        receiver: RTPHost {
            parameters:
                forwarding = false;
                profileName = "inet.transportlayer.rtp.RTPAVProfile";
                destinationAddress = "sender";
                portNumber = 5004;
                bandwidth = 8000;
                fileName = "";
                payloadType = 32;
                @display("p=54,128");
        }
        configurator: IPv4NetworkConfigurator {
            parameters:
                @display("p=54,23");
        }
    connections:
        sender.pppg++ <--> ethernet <--> receiver.pppg++;
}
