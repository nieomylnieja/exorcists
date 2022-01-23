package main

import "math/rand"

type Message struct {
	Timestamp    int
	Source       int
	Type         MessageType
	Prop         PropType
	HouseNum     int
	ResourceType ResourceType
}

func (m *Message) WithProp(prop PropType) *Message {
	return &Message{Timestamp: m.Timestamp, Source: m.Source, Type: m.Type, HouseNum: m.HouseNum,
		Prop:         prop,
		ResourceType: PropResource}
}

func (m *Message) WithHouseNum(houseNum int) *Message {
	return &Message{Timestamp: m.Timestamp, Source: m.Source, Type: m.Type, Prop: m.Prop,
		HouseNum:     houseNum,
		ResourceType: HouseResource}
}

func (e *Exorcist) Message(typ MessageType) *Message {
	return &Message{Timestamp: e.Timestamp, Source: e.Rank, Type: typ}
}

func (e *Exorcist) SendTo(dest int, msg *Message) {
	e.Chans[dest] <- msg
}

func (e *Exorcist) sendPropRequests() {
	for rank := range e.Chans {
		if e.Rank != rank {
			go e.SendTo(rank, e.Message(REQ_PROP).WithProp(MistGenerator))
		}
	}
}

func (e *Exorcist) sendHouseRequests() {
	var houseToBeHaunted int
	for {
		houseToBeHaunted = rand.Intn(e.AvailableHousesNum)
		if e.HouseAvailability[houseToBeHaunted] == -1 {
			break
		}
	}
	e.MarkHouseUnavailable(e.Rank, houseToBeHaunted)
	for rank := range e.Chans {
		if e.Rank != rank {
			go e.SendTo(rank, e.Message(REQ_HOUSE).WithHouseNum(houseToBeHaunted))
		}
	}
}

func (e *Exorcist) sendRelease() {
	for rank := range e.Chans {
		if e.Rank != rank {
			go e.SendTo(rank, e.Message(RELEASE))
		}
	}
}
