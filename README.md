# C3 - Camel's C2 Link Library

C3 is a system for passing messages between two machines (local and remote).
C3 was created to ease and partially automate the programming of C2 (command and
		control) systems in robotics projects. 

C3 messages can contain any combination of integers, strings, byte lists and floats. C3 can
also be used to define enumerations and can send enum types too.

Quite often, the data that is being sent to a remote machine is a global setting
that needs to be saved somehow. C3 generates a Settings singleton class and
automates the extraction of settings to it.

C3 has these features
<ul>
<li>C++ (header only) and Javascript APIs</li>
<li>Easy to use</li>
<li>Checksums and acknowledge fields ensure data integrity</li>
<li>Automatic extraction of settings from messages</li>
<li>Lean code generation</li>
</ul>


# The Process

<ul>
<li>Create a JSON packets file that defines enums and messages</li>
<li>Use the Python script to generate a C++ API</li>
<li>Include the generated C++ header files in your embedded microcontroller project</li>
<li>Use either the C++ header files, or NodeJS with the Javascript API to communicate with your device</li>
</ul>




# Example JSON Packets File

This example is for a fancy blinky light system. It can blink lights in different colours
and with different patterns. The different patterns are defined by the "PATTERN" enum in the enums section.
The packets section defines four messages that are used to send telemetry data from the remote device
to the local/controlling machine. The local machine can also send commands that set the colour and pattern
of the blinky lights. There's also a set_delay setting message that could set the speed of the blinky patterns.

blinky.json :
```
{
	"project": {
		"title": "Blinky",
			"description": "The protocol description for a blinky light gadget.",
			"version": "1.0"
	},
		"enums": [{
			"title": "PATTERN",
			"description": "The lighting pattern",
			"fields": [
				"BLINKY",
			"CHASE",
			"FLIP_FLOP",
			"UNKNOWN"
			]
		},
		{
			"title": "COLOR",
			"description": "light colours",
			"fields": [
				"RED",
			"GREEN",
			"BLUE"
			]
		}
		],
		"packets": [{
			"title": "telemetry",
			"description": "periodic telemetry",
			"setting": "false",
			"owner": "remote",
			"ack_required": "false",
			"fields": [{
				"name": "ticks",
				"type": "uint32_t"
			},
			{
				"name": "red",
				"type": "COLOR"
			},
			{
				"name": "green",
				"type": "COLOR"
			},
			{
				"name": "blue",
				"type": "COLOR"
			},
			{
				"name": "pattern",
				"type": "PATTERN"
			}
			]
		},
		{
			"title": "set_color",
			"description": "Sets the light color",
			"setting": "false",
			"owner": "local",
			"ack_required": "true",
			"fields": [{
				"name": "color",
				"type": "COLOR"
			}
			]
		},
		{
			"title": "set_pattern",
			"description": "sets the pattern type",
			"setting": "false",
			"owner": "local",
			"ack_required": "true",
			"fields": [{
				"name": "pattern",
				"type": "PATTERN"
			}]
		},
		{
			"title": "set_delay",
			"description": "sets the speed of the light blinks",
			"setting": "true",
			"owner": "local",
			"ack_required": "true",
			"fields": [{
				"name": "delay",
				"type": "uint32_t"
			}]
		}
		]
}

```

# Generating A C++ API

<ul>
<li>Make sure you have Python3 installed. </li>
<li>Run `./builder.py blinky.json` </li>
<li>The C++ API will be created in ./c3 </li>
</ul>

# Example C++ (Arduino) Remote Machine

The generated C++ API depends on [etk](https://github.com/supercamel/EmbeddedToolKit)
so you will need to get that.

```
#include <c3.h>

typedef ccc::C3Link<class Coms, ccc::REMOTE> C3;

class Coms : public C3
{
	public:

	private:
		friend C3;

		void set_color_pack_handler(ccc::set_color_pack& pack) {
			auto color = pack.color;
			// do something to set the color
		}

		void set_pattern_pack_handler(ccc::set_pattern_pack& pack) {
			auto pattern = pack.pattern;
			// do something with pattern
		}

		void on_setting_changed(uint16 msg_id) {
			auto& settings = ccc::Settings::get();
			// save settings to flash/eeprom
		}
		void put(char c) {
			Serial.write(c);
		}

		uint8 get() {
			return Serial.read();
		}

		bool available() {
			return (Serial.available() != 0);
		}

};

Coms coms;

void setup() {
	Serial.begin(57600);
}

void loop() {
	auto& settings = ccc::Settings::get();
	// do something with settings.delay
	coms.read();
	coms.serialise();
}


```

# Example C++ Local Machine

```
#include <iostream>
#include "serial.h"
#include "../c3/c3.h"

using namespace std;


class Local : public ccc::C3Link<Local, ccc::LOCAL>
{
	public:
		void telemetry_pack_handler(auto& pack)
		{
			cout << "ticks: " << pack.ticks << endl;
		}

		void put(char c) {
			Serial.write(c);
		}

		uint8 get() {
			return Serial.read();
		}

		bool available() {
			return (Serial.available() != 0);
		}
};

int main()
{
	Local local;

	//creates a packet for sending, and returns a pointer to it.
	auto color_pack = local.create_packet<ccc::set_color>();
	//ALWAYS check for a nullptr. C3 uses a small memory pool to store packets.
	// it may run out of memory!!
	if(color_pack != nullptr)
	{
		color_pack->red = 1.0;
		color_pack->green = 0.5;
		color_pack->blue = 0.8;
	}

	while(true)
	{
		local.read();
		local.serialise();
		sleep_ms(100);
	}
}

```

# NodeJS Example

. . . on the way
