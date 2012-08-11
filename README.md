# Duration

Duration is an application for time.

current version is 001
# OSC protocol

Duration sends arbitrary timeline values over OSC using the following format

/duration/version/tracktype/trackname/ timestamp<uint64_t> args<...>

Bang:
/duration/001/bangs/trackname timestamp<uint64_t>

Flag:
/duration/001/flags/trackname timestamp<uint64_t> flagname<string>

Curve
/duration/001/curves/trackname timestamp<uint64_t> value<float> min<float> max<float>

Switch:
/duration/001/switches/trackname timestamp<uint64_t> value<int> on = 1 off = 0