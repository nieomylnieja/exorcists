package main

type State uint8

const (
	GATHERING_PROPS State = iota + 1
	WAITING_FOR_FREE_PROPS
	RESERVING_HOUSE
	WAITING_FOR_FREE_HOUSE
	HAUNTING_HOUSE
)

func (s State) String() string {
	switch s {
	case GATHERING_PROPS:
		return "Gathering Props"
	case WAITING_FOR_FREE_PROPS:
		return "Waiting For Free Props"
	case RESERVING_HOUSE:
		return "Reserving House"
	case WAITING_FOR_FREE_HOUSE:
		return "Waiting For Free House"
	case HAUNTING_HOUSE:
		return "Haunting House"
	default:
		return "None"
	}
}
