# Exorcists

Project for Distributed Programming course at Poznan University of Technology.

## The program

Make sure you have the `MPI` libs installed, `mpicc` for building and for `mpirun` running . You can either use the
provided `Makefile` or `cmake` for your convenience.

### Running

The makefile provides the `run` and `run-with-config` targets, the latter will pass the env configuration specified
in `.env` file unless you create `.local.env` file (you can use `make local-env`). Any of the envs described in
the `.env` file can be used passed to the program to modify its execution:

- `NP` is the number of processes to run the MPI with.
- `LOG_LEVEL` is the log level to run the program with.
- `MIST_GENERATORS_NUM` is the number of mist generators in the prop warehouse.
- `TAPE_RECORDER_NUM` is the number of tape recorders "Kasprzak" in the prop warehouse.
- `SHEETS_NUM` is the number of used-up sheets in the prop warehouse.
- `HOUSE_NUM` is the number of houses in the hood.
- `MIN_HAUNTING_TIME_SECONDS` is the minimal time in seconds to haunt the house.
- `MAX_HAUNTING_TIME_SECONDS`is the maximal time in seconds to haunt the house.

### Logging

You can change the log level with `LOG_LEVEL` env. It accepts `debug`, `info` and `error` levels (case-insensitive). The
default log level is `info`.

### Debug

For debugging check out the `make debug` target, which will run `valgrind` for you with 2 processes.

## The Problem

### Description [Polish]

```text
Grupka egzorcystów, sfrustrowana brakiem duchów,
postanawia nieco sobie dopomóc w interesie i przy pomocy urządzeń z
magazynu (do wytwarzania sztucznej mgły, magnetofonów marki Kasprzak
oraz zużytych prześcieradeł z domu opieki społecznej) pokryjomu
urządza "nawiedzenia" wybranych domów. Z uwagi na ograniczoną liczbę
rekwizytów oraz z uwagi na fakt, że równocześnie nie powinni egzorcyści 
nawiedzać tych samych domów, konieczne okazało się zaimplementowanie
mobilnej aplikacji szeregującej nawiedzanie duchów.
```

### English translation

Group of exorcists is frustrated with the lack of ghosts, they decide to help themselves out a bit using the devices
stored in the local warehouse. These include:

- artificial mist generator
- tape recorded "Kasprzak"
- worn-out bedsheets from the local nursing home

Secretly they start fake-haunting the selected houses. Due to the limited number of props and having in mind that the
any two exorcists shouldn't be visiting the same house It was deemed necessary to create a mobile app for managing the
to-be-haunted houses.

### Data

- M: artificial mist generators
- K: "Kasprzak" tape recorders
- P: used up sheets
- D: houses

### Constraints

- M < K < P < D
- to haunt the house, the E must first acquire all the props, only then can he book the house using the app
- once the house has been haunted It must suffer a delay before the next haunting

## The Algorithm

### Important concepts

### Clock

We're using a simple operations counter. Each exorcist will increment the clock when:

- Reserving the props.
- Reserving the house.

Unlike Lamport's clock we're not updating the clock in regard to the incoming requests. This mechanism is sufficient for
ensuring equal share of props, It really works just like a queue.

### Description

Since the props can be sorted by quantity and none of the sets are equally large we should require the exorcists to
obtain the props in order. This way once an exorcist obtains the mist generator he can safely take hold of both the tape
recorder and the sheet. This simplifies the problem, and means that we can just ignore the other props in the
implementation.

We'll be using Lamport's logical clock to determine the priority of the messages. Everyone stores their own version of
the world, this includes the supply of props in the warehouse. Each exorcist tries to take the hold of the mist
generator, requests are sent to all the other exorcists, which return:

- ACK
    - if the requesting exorcists timestamp was higher or if the timestamps were equal, but our rank was higher.
    - the respondent will decrement the prop count unless he already received a request from this exorcist and responded
      with ACK.
- NACK
    - if their timestamp was lower
    - no action required

We start with `m` mist generators, we'll assume `n` is the number of NACKs gathered. When the exorcist has gathered all
responses to his request he must check if `m - n > 0`. If the condition is met he grabs a piece of that misty generator.
He also grabs the other two props since the problem's constraints allow it.

Finally, he may enter the house. Before we do that we have to make sure the house is not haunted by someone else, we
also want to achieve an even house haunting spread, so that each house is haunted roughly equal amount of times. Each
exorcist keeps track of whose haunting each house so he does not ask for a haunted house.

We randomly choose the house from the available ones. We send `REQ_PROP` with the house number to all the exorcists, we
have to receive `ACK` from everyone in order to make the reservation for the house, If we receive a `NACK`, we update
the list and proceed with a new free random house number. When we exit the house, we send `RELEASE`. Everyone should
then update their lists. If we're receiving `REQ_HOUSE` we should update our list with the house number received (unless
we're also fighting for the access to that house or haunting the house). If we receive `REQ_HOUSE` on the house before
processing the `RELEASE`, well... we might have to try our luck with another house, no big deal.

As to the fate of those who were not as fortunate. They must wait for `RELEASE` from one of the exorcists currently
haunting the houses. They do not need to resend all the requests, they are already "queued" in line. The important bit
here is that they can (and should If need be) decrement the prop count below zero. Likewise, each time an exorcist
leaves the haunted house (depositing the props at the warehouse) and sends `RELEASE` to them, they should increment
their prop count. If the prop count climbs above zero, they're ready to go, no further checks required --> It's their
turn.

The exorcists haunting a house may not claim anything in advance, since we don't know how long will It take them to
haunt the house. They will return `ACK` and `NACK` adhering to the same rules of timestamp comparison to their succeeded
request as long as they are haunting the house, they will decrement their prop count each time, they will also increment
their prop count in response to `ACK` from someone who finished the haunting. All in all they act just the same as the
exorcists struggling to get their hands on some props.

The `RELEASE` for the house and prop is sent as a single message. The rank of the process sending `RELEASE` is enough to
update the houses and props availability.

### Implementation

We use `resources_state` array to keep track of who is haunting each house and who has taken the props, this also helps
us with not decrementing the resources twice (for `ACK` sent and `NACK` received). The example below presents a sample
`resources_state` for one of exorcist with rank `0` who is not haunting any house. The exorcist `1` is not haunting
either, however he already grabbed the required props. The last one `2` is haunting the house `0`.

```text
[
    0: {house_n: -1, mist_generator: 0},
    1: {house_n: -1, mist_generator: 1},
    2: {house_n:  0, mist_generator: 1},
]
```

The struct responsible for holding that information is:

```c
typedef struct {
    int house_n;
    int mist_generator;
} resource_t;
```

- `house_n` is the house number, `-1` if the exorcist is not haunting any house.
- `mist_generator` is a flag, if the exorcist has it, it's `1` else it's `0`.

We first handle the message types:

- `REQ_PROP`
- `REQ_HOUSE`
- `ACK`
- `NACK`
- `RELEASE`

And only then we deal with potential state changes:

- `GATHERING_PROPS`
- `WAITING_FOR_FREE_PROPS`
- `RESERVING_HOUSE`
- `WAITING_FOR_FREE_HOUSE`
- `HAUNTING_HOUSE`

What's great about this design is that everything happens sequentially within the scope of a single exorcist process,
the only concurrent part here is the haunting. This process when finished haunting will set up everything for the
exorcist to start gathering props.