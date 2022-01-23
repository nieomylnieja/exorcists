package main

import (
	"math/rand"
	"sync"
	"time"
)

func main() {
	size := 2
	wg := new(sync.WaitGroup)
	wg.Add(size)

	chans := make([]chan *Message, size)
	for i := range chans {
		chans[i] = make(chan *Message, size)
	}

	for rank := 0; rank < size; rank++ {
		e := NewExorcist(size, rank, chans)
		go e.Run(wg)
	}
	wg.Wait()
}

func NewExorcist(size, rank int, chans []chan *Message) *Exorcist {
	tapeRecordersAvailability := make(map[Rank]int, size)
	housesAvailability := make(map[Rank]int, size)
	for i := 0; i < size; i++ {
		tapeRecordersAvailability[i] = 0
		housesAvailability[i] = -1
	}
	return &Exorcist{
		State:                    GATHERING_PROPS,
		Timestamp:                0,
		Size:                     size,
		Rank:                     rank,
		Chans:                    chans,
		TapeRecorderAvailability: tapeRecordersAvailability,
		AvailableTapeRecorders:   size - 1,
		HouseAvailability:        housesAvailability,
		AvailableHousesNum:       size + 2,
		AvailableHouses:          size + 2,
	}
}

type Rank = int
type Exorcist struct {
	// Rank and Size are set by MPI.
	Rank Rank
	Size int
	// Timestamp is a pseudo Lamport's clock implementation.
	Timestamp int
	// State we're in, should be changed atomically.
	State State
	// Chans used for communicating with other processes.
	Chans []chan *Message
	// TapeRecorderAvailability keeps track of who has taken the TapeRecorder.
	// We don't care about other props as the algorithm constraints pose no problems over our solution.
	TapeRecorderAvailability map[Rank]int
	// AvailableTapeRecorders is a helper to the TapeRecorderAvailability, we could read the count from there,
	// but keeping it separately is cheaper and clearer.
	AvailableTapeRecorders int
	// GatheredResponses helps both with GATHERING_PROPS and RESERVING_HOUSE to know when we've gathered all responses.
	GatheredResponses int
	// HouseAvailability keeps track of who is haunting which house. -1 denotes the house is free.
	HouseAvailability map[Rank]int
	// AvailableHouses is a logical copy of AvailableTapeRecorders for the houses.
	AvailableHouses int
	// AvailableHousesNum informs how many houses in total do we have.
	AvailableHousesNum int
}

func (e *Exorcist) Run(wg *sync.WaitGroup) {
	defer wg.Done()
	e.sendPropRequests()
	e.listen()
}

func (e *Exorcist) listen() {
	for {
		m := <-e.Chans[e.Rank]
		e.handleMessage(m)
	}
}

func (e *Exorcist) handleMessage(m *Message) {
	// handle the message in a state agnostic fashion first.
	switch m.Type {
	case REQ_PROP:
		e.handleReqProp(m)
	case REQ_HOUSE:
		e.handleReqHouse(m)
	case ACK:
		e.handleAck(m)
	case NACK:
		e.handleNack(m)
	case RELEASE:
		e.handleRelease(m)
	}

	// deal with the current state potential changes.
	switch e.State {
	case GATHERING_PROPS:
		e.handleGatheringProps()
	case WAITING_FOR_FREE_PROPS:
		e.handleWaitingForFreeProps()
	case RESERVING_HOUSE:
		e.handleReservingHouse()
	case WAITING_FOR_FREE_HOUSE:
		e.handleWaitingForFreeHouse()
	case HAUNTING_HOUSE:
		e.handleHauntingHouse()
	}
}

func (e *Exorcist) startHaunting() {
	hauntingTime := time.Duration(rand.Intn(5)+2) * time.Second
	e.Logf(nil, "started haunting house for: %s", hauntingTime)
	time.Sleep(hauntingTime)

	e.MarkHouseAvailable(e.Rank)
	if e.AvailableTapeRecorders > 0 {
		e.State = GATHERING_PROPS
	} else {
		e.State = WAITING_FOR_FREE_PROPS
	}

	e.sendRelease()
}
