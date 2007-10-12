/*
This file defines a Promela model for xv6's
acquire, release, sleep, and wakeup, along with
a model of a simple producer/consumer queue.

To run:
	spinp sleep1.p

(You may need to install Spin, available at http://spinroot.com/.)

After a successful run spin prints something like:

	unreached in proctype consumer
		(0 of 37 states)
	unreached in proctype producer
		(0 of 23 states)

After an unsuccessful run, the spinp script prints
an execution trace that causes a deadlock.

The safe body of producer reads:

		acquire(lk);
		x = value; value = x + 1; x = 0;
		wakeup(0);
		release(lk);
		i = i + 1;

If this is changed to:

		x = value; value = x + 1; x = 0;
		acquire(lk);
		wakeup(0);
		release(lk);
		i = i + 1;

then a deadlock can happen, because the non-atomic
increment of value conflicts with the non-atomic 
decrement in consumer, causing value to have a bad value.
Try this.

If it is changed to:

		acquire(lk);
		x = value; value = x + 1; x = 0;
		release(lk);
		wakeup(0);
		i = i + 1;

then nothing bad happens: it is okay to wakeup after release
instead of before, although it seems morally wrong.
*/

#define ITER 4
#define N 2

bit lk;
byte value;
bit sleeping[N];

inline acquire(x)
{
	atomic { x == 0; x = 1 }
}

inline release(x)
{
	assert x==1;
	x = 0
}

inline sleep(cond, lk)
{
	assert !sleeping[_pid];
	if
	:: cond ->
		skip
	:: else ->
		atomic { release(lk); sleeping[_pid] = 1 };
		sleeping[_pid] == 0;
		acquire(lk)
	fi
}

inline wakeup()
{
	w = 0;
	do
	:: w < N ->
		sleeping[w] = 0;
		w = w + 1
	:: else ->
		break
	od
}

active[N] proctype consumer()
{
	byte i, x;
	
	i = 0;
	do
	:: i < ITER ->
		acquire(lk);
		sleep(value > 0, lk);
		x = value; value = x - 1; x = 0;
		release(lk);
		i = i + 1;
	:: else ->
		break
	od;
	i = 0;
	skip
}

active[N] proctype producer()
{
	byte i, x, w;
	
	i = 0;
	do
	:: i < ITER ->
		acquire(lk);
		x = value; value = x + 1; x = 0;
		release(lk);
		wakeup();
		i = i + 1;
	:: else ->
		break
	od;
	i = 0;
	skip	
}

