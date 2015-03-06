uREST
=====

uREST is a simple sketch that show the power of uHTTP library. It is very simple to create a REST like interface
for your arduino project.

This skecth is written by Filippo Sallemi <fsallemi@nomadnt.com>

REST interface
--

| METHOD | URI           | DESCRIPTION                    |
| ------ | ------------- | ------------------------------ |
| GET    | /digital/     | Return all digital pins status |
| GET    | /digital/{id} | Return digital pin status      |
| PUT    | /digital/{id} | Set digital pin status         |
| GET    | /analog/      | Return all digital pins status |
| GET    | /analog/{id}  | Return digital pin status      |
| PUT    | /analog/{id}  | Set digital pin status         |

cURL examples
--

Get all digital pins status

```bash
curl http://192.168.0.254/digital/
```
```json
[{"id": 1, "pin": 2, "value": 0, "pwm": 0}, {"id": 2, "pin": 3, "value": 128, "pwm": 1}, {"id": 3, "pin": 4, "value": 0, "pwm": 0}, {"id": 4, "pin": 5, "value": 0, "pwm": 1}, {"id": 5, "pin": 6, "value": 0, "pwm": 1}, {"id": 6, "pin": 7, "value": 0, "pwm": 0}, {"id": 7, "pin": 8, "value": 0, "pwm": 0}, {"id": 8, "pin": 9, "value": 0, "pwm": 97}]
```

Get digital pin with id 3 status

```bash
curl http://192.168.0.254/digital/3
```
```json
{"id": 3, "pin": 4, "value": 0, "pwm": 0}
```

Set digital pin with id 3 to HIGH

```bash
curl -X PUT -d "value=1" http://192.168.0.254/digital/3
```
```json
{"id": 3, "pin": 4, "value": 1, "pwm": 0}
```

Set pwm pin with id 3 to 127

```bash
curl -X PUT -d "value=127" http://192.168.0.254/digital/2
```
```json
{"id": 2, "pin": 3, "value": 127, "pwm": 1}
```

Read all analog pins value
```bash
curl http://192.168.0.254/analog/
```
```json
[{"id": 1, "pin": 1, "value": 145}, {"id": 2, "pin": 2, "value": 0}, {"id": 3, "pin": 3, "value": 0}, {"id": 4, "pin": 4, "value": 0}, {"id": 5, "pin": 5, "value": 0}, {"id": 6, "pin": 6, "value": 0}]
```

<p align="right"><a href='https://pledgie.com/campaigns/28538'>
    <img alt='Click here to lend your support to: uHTTP and make a donation!' src='https://pledgie.com/campaigns/28538.png?skin_name=chrome' border='0' >
</a></p>
