# sensor_client
Websocket based client for json sensor data provider

## dependencies
`libboost-system-dev`
`libcpprest-dev`
`libfmt-dev`

## build
`cmake <project_path>`
`cd <build_path> && make all`

## known_issues
1. no reconnect option, user can catch **web::websockets::client::websocket_exception** which contents **set_fail_handler:**, then try to connect again
2. bad sensorConnected() logic, now checks only what client knows about sensors been connected or not, updated from incoming messages
