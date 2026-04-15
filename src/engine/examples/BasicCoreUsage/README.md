# BasicCoreUsage

This example shows the EngineSquared core loop, entity/component setup, scheduler registration, and shutdown handling.

## What it shows

- Create entities and attach components.
- Register systems on the `Startup`, `Update`, and `Shutdown` schedulers.
- Use a resource shared by systems.
- Stop the loop when a condition is met.

The example uses a school scenario to make the flow easy to follow.

## Run the example

From the repository root:

```bash
xmake f -c
xmake f --BasicCoreUsage=y -y
xmake run BasicCoreUsage
```

## Expected output

The program logs:

```console
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Arrived at the school.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Arrived at the school.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Arrived at the school.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Monitoring the school activities.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Monitoring the school activities.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Going to a meeting.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Back from the meeting.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Monitoring the school activities.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Monitoring the school activities.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Pugo ]: Work time is over! Going home!
[xxxx-xx-xx xx:xx:xx.xxx] [info] [The A]: Leaving the school.
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Working...
[xxxx-xx-xx xx:xx:xx.xxx] [info] [Tom  ]: Work time is over! Going home!
[xxxx-xx-xx xx:xx:xx.xxx] [info] Everyone left the school. Shutting down the lights and locking the doors.
```

