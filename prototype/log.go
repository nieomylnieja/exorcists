package main

import (
	"fmt"
	"os"
	"strconv"

	"github.com/mattn/go-colorable"
	json "github.com/neilotoole/jsoncolor"
)

func (e *Exorcist) Logf(m *Message, format string, a ...interface{}) {
	recordersMap := make(map[string]int, len(e.TapeRecorderAvailability))
	for k, v := range e.TapeRecorderAvailability {
		recordersMap["Rank "+strconv.Itoa(k)] = v
	}
	housesMap := make(map[string]string, len(e.HouseAvailability))
	for k, v := range e.HouseAvailability {
		houseStr := "House " + strconv.Itoa(v)
		if v == -1 {
			houseStr = "Not haunting"
		}
		housesMap["Rank "+strconv.Itoa(k)] = houseStr
	}
	out := colorable.NewColorable(os.Stdout)
	enc := json.NewEncoder(out)
	enc.SetColors(json.DefaultColors())
	enc.SetIndent("", "  ")
	logObj := map[string]interface{}{
		"E": map[string]interface{}{
			"timestamp":         e.Timestamp,
			"rank":              e.Rank,
			"state":             e.State.String(),
			"tapeRecorders":     e.AvailableTapeRecorders,
			"tapeRecordersList": recordersMap,
			"availableHouses":   e.AvailableHouses,
			"rankToHouseNumber": housesMap,
		},
		"info": fmt.Sprintf(format, a...),
	}
	if m != nil {
		logObj["message"] = map[string]interface{}{
			"timestamp":    m.Timestamp,
			"source":       m.Source,
			"type":         m.Type.String(),
			"prop":         m.Prop.String(),
			"houseNum":     m.HouseNum,
			"resourceType": m.ResourceType.String(),
		}
	}
	enc.SetTrustRawMessage(true)
	_ = enc.Encode(logObj)
}
