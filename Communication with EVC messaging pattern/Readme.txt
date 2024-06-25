

1)Analog input board just waits for messages on wakeup
2)EVC will send opening request by simply asking for Data
3)Analog input board will send data from all the ports regardless of what port EVC require data from
4)that computation will be done on EVC itself, this makes the communication super simple

EVC opening request Data:
/?senddata<LF> 


Analog input response

<SOH>IN1:<Analogdata>,IN2:<Analogdata>,IN3:<Analogdata>,IN4:<Analogdata>!<LF>

if error

<SOH>IN1:ERROR,IN2:ERROR,IN3:ERROR,IN4:ERROR.<LF>


this will show error if on any channel error occurs

On EVC we will only care for error, for which user has enabled the channels
