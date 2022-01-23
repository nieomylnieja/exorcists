package main

func (e *Exorcist) handleReqProp(msg *Message) {
	var responseType MessageType
	if msg.Timestamp < e.Timestamp || (msg.Timestamp == e.Timestamp && msg.Source > e.Rank) {
		responseType = ACK
		e.RemoveTapeRecorderFromWarehouse(msg.Source)
	} else {
		responseType = NACK
	}
	e.Logf(msg, "responded with: %s", responseType)
	e.SendTo(msg.Source, e.Message(responseType).WithProp(msg.Prop))
}

func (e *Exorcist) handleReqHouse(msg *Message) {
	var responseType MessageType
	if e.HouseAvailability[e.Rank] != -1 {
		if e.State == RESERVING_HOUSE &&
			(msg.Timestamp < e.Timestamp ||
				(msg.Timestamp == e.Timestamp && msg.Source > e.Rank)) {
			responseType = ACK
			e.MarkHouseUnavailable(msg.Source, msg.HouseNum)
		} else {
			responseType = NACK
		}
	} else {
		responseType = ACK
		e.MarkHouseUnavailable(msg.Source, msg.HouseNum)
	}
	e.Logf(msg, "responded with :%s", responseType)
	e.SendTo(msg.Source, e.Message(responseType).WithHouseNum(msg.HouseNum))
}

func (e *Exorcist) handleAck(msg *Message) {
	e.GatheredResponses++
	e.Logf(msg, "received ACK")
}

func (e *Exorcist) handleNack(msg *Message) {
	e.GatheredResponses++
	switch e.State {
	case RESERVING_HOUSE:
		e.MarkHouseUnavailable(msg.Source, msg.HouseNum)
	case GATHERING_PROPS:
		e.RemoveTapeRecorderFromWarehouse(msg.Source)
	}
	e.Logf(msg, "received NACK")
}

func (e *Exorcist) handleRelease(msg *Message) {
	e.MarkHouseAvailable(msg.Source)
	e.AddTapeRecorderToWarehouse(msg.Source)
	e.Logf(msg, "I received a release!")
}
