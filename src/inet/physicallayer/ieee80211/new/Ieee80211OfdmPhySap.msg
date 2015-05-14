//
// IEEE 802.11-2012 speciciation
// 7. PHY service specification  
//

//
// 7.3.4.4 PHY-SAP service primitives parameters
//

enum ChannelOffset
{
    CH_OFFSET_NONE = 0;
    CH_OFFSET_ABOVE = 1;
    CH_OFFSET_BELOW = 2;
}

class TxVector
{
    int length;
    double datarate;
}

class Status
{
}

class RxVector
{
    int length;
    double datarate;
}

class RxError
{
}

class IpiState
{
}

class IpiReport
{
}

class PhyConfigVector
{
    uint8_t activeRxChainSet;
    int operatingChannel;
    int channelOffset @enum(ChannelOffset);
}

class TxStatus
{
}

class ChannelList
{
}

//
// 7.3.4.2 PHY-SAP peer-to-peer service primitives
// 7.3.5 PHY-SAP detailed service specification
//

class PhyDataRequest
{
    char data[];
}

class PhyDataIndication
{
    char data[];
}

class PhyDataConfirm
{
}

//
// 7.3.4.3 PHY-SAP sublayer-to-sublayer service primitives
// 7.3.5 PHY-SAP detailed service specification
//

class PhyTxStartRequest
{
    TxVector txVector;
}

class PhyTxStartConfirm
{
    TxStatus txStatus;
}

class PhyTxEndRequest
{
}

class PhyTxEndConfirm
{
}

class PhyCcaResetRequest
{
    IpiState ipiState;
}

class PhyCcaResetConfirm
{
    IpiState ipiState;
    IpiReport ipiReport;
}

class PhyCcaIndication
{
    Status status;
    IpiReport ipiReport;
    ChannelList channelList;
}

class PhyRxStartIndication
{
    RxVector rxVector;
}

class PhyRxEndIndication
{
    RxError rxError;
    RxVector rxVector;
}

class PhyConfigRequest
{
    PhyConfigVector phyConfigVector;
}

class PhyConfigConfirm
{
}
