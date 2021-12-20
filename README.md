# Exorcists

Project for Distributed Programming course at Politechnika Poznańska.

## Running

Make sure you have the MPI libs installed for building (along with `mpicc`) and for running `mpirun`.

You can either use the provided `Makefile` or `cmake`. The makefile also provides the `run` target.

## Description

### English translation

Group of exorcists is frustrated with the lack of ghosts, they decide to help themselves out a bit using the devices
stored in the local warehouse. These include:

- artificial mist generator
- tape recorded "Kasprzak"
- worn-out bed sheets from the local nursing home

Secretly they start fake-haunting the selected houses. Due to the limited number of props and having in mind that the
any two exorcists shouldn't be visiting the same house It was deemed necessary to create a mobile app for managing the
to-be-haunted houses.

### Original

```text
Grupka egzorcystów, sfrustrowana brakiem duchów, postanawia nieco sobie dopomóc w interesie i przy pomocy urządzeń z
magazynu (do wytwarzania sztucznej mgły, magnetofonów marki Kasprzak oraz zużytych prześcieradeł z domu opieki
społecznej) pokryjomu urządza "nawiedzenia" wybranych domów. Z uwagi na ograniczoną liczbę rekwizytów oraz z uwagi na fakt, że
równocześnie nie powinni egzorcyści nawiedzać tych samych domów, konieczne okazało się zaimplementowanie mobilnej
aplikacji szeregującej nawiedzanie duchów.
```

### Data

- K: "Kasprzak" tape recorders
- M: artificial mist generators
- P: used up sheets
- D: houses

### Limitations

- M < K < P < D
- to haunt the house, the exorcist must first acquire all the props, only then can he book the house using the app
- once the house has been haunted It must suffer a delay before the next haunting

## Algorithm

Since the props can be sorted by quantity and none of the sets are equally large we should require the exorcists to
obtain the props in order. This way once an exorcist obtains the mist generator he can safely take hold of both the tape
recorder and the sheet.

We'll be using Lamport's logical clock to determine the priority of the messages. Everyone stores their own version of
the world, this includes the supply of props in the warehouse. Each exorcist tries to take the hold of the mist
generator, requests are sent to all other exorcists, which return:

- ACK
    - if the requesting exorcist's timestamp was higher
    - the respondent must decrement the prop count
- NACK
    - if their timestamp was lower
    - no action required

We start with `m` mist generators, we'll assume `n` is the number of NACKs gathered. When gathering the responses to his
request, the exorcist must check if `m - n > 0`, If at any time he calculates that even If all the pending responses are
NACKs, but he will still meet the condition, he may proceed. If he does may grab a piece of that misty generator.
Furthermore, he now knows at least on of both the tape recorded and the sheet are available, so he grabs them too.

Finally, he may enter the house. We know that at least one house is available we can easily choose the house using
Lamport's clock value for the request we've succeeded with: `d = c mod D`, where `c` is the clock value, `D` is the
number of houses available and `d` is the house number (starting from `0` to `D - 1`).

As to the fate of those who were not as fortunate. They must wait for the signal from one of the exorcists currently
haunting the houses. They do not need to resend all the requests, they are already "queued" in line. The important bit
here is that they can (and should If need be) decrement the prop count below zero. Likewise, each time an exorcist
leaves the haunted house (depositing the props at the warehouse) and sends an ACK to them, they should increment their
prop count. If the prop count climbs above zero, they're ready to go, no further checks required.

The exorcists haunting may not claim anything in advance, since we don't know how long will It take them to haunt the
house. They will return ACK and NACK adhering to the same rules of Lamport's clock comparison to their succeeded request
as long as they are haunting the house, they will decrement their prop count each time (unless these are the pending
responses they awaited before entering), they will also increment their prop count in response to ACK from someone who
finished the haunting. All in all they act just the same as the exorcists struggling to get their hands on some props.

Once the exorcist leaves the house, he sends an ACK informing others the prop is free, he will also increment his own
counter. He then starts all over again, he will not wait until there's a mist generator available, he will instantly
send the requests to all the others, this will "queue" him according to the request's precedence with Lamport'
s clock.
