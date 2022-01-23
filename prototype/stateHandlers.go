package main

func (e *Exorcist) handleGatheringProps() {
	if e.GatheredResponses == e.Size-1 {
		e.GatheredResponses = 0
		if e.AvailableTapeRecorders > 0 {
			e.Timestamp++
			if e.AvailableHouses > 0 {
				e.State = RESERVING_HOUSE
				e.sendHouseRequests()
			} else {
				e.State = WAITING_FOR_FREE_HOUSE
			}
		} else {
			e.State = WAITING_FOR_FREE_PROPS
		}
	}
}

func (e *Exorcist) handleWaitingForFreeProps() {
	if e.AvailableTapeRecorders > 0 {
		e.State = GATHERING_PROPS
		e.sendPropRequests()
	}
}

func (e *Exorcist) handleReservingHouse() {
	if e.GatheredResponses == e.Size-1 {
		e.GatheredResponses = 0
		if e.AvailableHouses > 0 {
			e.Timestamp++
			e.State = HAUNTING_HOUSE
			go e.startHaunting()
		} else {
			e.State = WAITING_FOR_FREE_HOUSE
		}
	}
}

func (e *Exorcist) handleWaitingForFreeHouse() {
	if e.AvailableHouses > 0 {
		e.State = RESERVING_HOUSE
		e.sendHouseRequests()
	}
}

func (e *Exorcist) handleHauntingHouse() {} // do nothing. The sleep routine will change the state.
