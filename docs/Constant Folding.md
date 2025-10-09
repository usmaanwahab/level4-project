Instead of doing a static calculation are runtime perform the calculation during compile time.

```c
int seconds_in_a_week = 60*60*24*7
```

At compile time, we can simplify this to
```c
int seconds_in_a_week = 604800;
```