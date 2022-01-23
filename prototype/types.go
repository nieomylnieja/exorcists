package main

type MessageType uint8

const (
	REQ_PROP MessageType = iota + 1
	REQ_HOUSE
	ACK
	NACK
	RELEASE
)

func (m MessageType) String() string {
	switch m {
	case REQ_PROP:
		return "REQ_PROP"
	case REQ_HOUSE:
		return "REQ_HOUSE"
	case ACK:
		return "ACK"
	case NACK:
		return "NACK"
	case RELEASE:
		return "RELEASE"
	default:
		return "None"
	}
}

type PropType uint8

const (
	MistGenerator PropType = iota + 1
	TapeRecorder
	UsedUpSheets
)

func (p PropType) String() string {
	switch p {
	case MistGenerator:
		return "Mist generator"
	case TapeRecorder:
		return "Tape recorder \"Kasprzak\""
	case UsedUpSheets:
		return "Used-up sheets"
	default:
		return "None"
	}
}

type ResourceType uint8

const (
	PropResource ResourceType = iota + 1
	HouseResource
)

func (r ResourceType) String() string {
	switch r {
	case PropResource:
		return "Prop"
	case HouseResource:
		return "House"
	default:
		return "None"
	}
}

func (e *Exorcist) RemoveTapeRecorderFromWarehouse(taker Rank) {
	if count := e.TapeRecorderAvailability[taker]; count == 0 {
		e.AvailableTapeRecorders--
	}
	e.TapeRecorderAvailability[taker] = 1
}

func (e *Exorcist) AddTapeRecorderToWarehouse(taker Rank) {
	e.AvailableTapeRecorders++
	e.TapeRecorderAvailability[taker] = 0
}

func (e *Exorcist) MarkHouseUnavailable(taker Rank, houseNumber int) {
	if houseNum := e.HouseAvailability[taker]; houseNum == -1 {
		e.AvailableHouses--
	}
	e.HouseAvailability[taker] = houseNumber
}

func (e *Exorcist) MarkHouseAvailable(taker Rank) {
	e.AvailableHouses++
	e.HouseAvailability[taker] = -1
}
