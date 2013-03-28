Godfrey is an agile robot designed for natural interactions with humans in their household environment. He will gracefully move around the home while answering questions, making small talk, and telling stories. Godfrey will wears his emotions on his sleeves. His design and development are in its early stages.

Godfrey is designed to experiment with how humans interact with intelligent agents. He is not an attempt to build a domestic servant nor is he an effort to build the one "true" AI architecture. I will leave these important goals to other, more capable, hands.

Cynthia Breazeal's wonderful [TED Talk](http://www.ted.com/talks/cynthia_breazeal_the_rise_of_personal_robots.html) supplies many examples of the kinds of exploration I would like to do with Godfrey.

You can follow the development of Godfrey on my [blog](http://myrobotgodfrey.blogspot.com/).


Goals
=====
* Godfrey's primary goal is to pull humans into a relationship with himself. This means he must be able to respond to human emotions and generate emotional cues that humans understand. He must also pursue independent goals and generally seem like a cognitive agent. A reactive planner similar in design to the CMU [Oz project](http://www.cs.cmu.edu/afs/cs/project/oz/web/oz.html) will control Godfrey's overall goals, behaviors and emotional responses. Oz's design has the advantage of being pragmatically engineered, very compatible with Godfrey's natural language component (see below) and it has mechanisms to infer and act on Godfrey's internal emotional states.

* Godfrey must understand and speak English. His design will make no attempt to solve the difficult speech recognition or the natural language understanding problems. He will instead use preexisting products and live within their limitations. My plan is to use [PocketSphinx](http://cmusphinx.sourceforge.net/2010/03/pocketsphinx-0-6-release/) to convert speech into text and [Flite](http://www.speech.cs.cmu.edu/flite/doc/index.html) to convert the response back into audio for human hearing. He will approach human language processing like a chatbot on steroids. Bruce Wilcox's [ChatScript](http://sourceforge.net/projects/chatscript/) is a great example of a modern chatbot design which, with some modification, can handle PocketSphinx's more limited vocabulary, weak natural language understanding and generation of responses. ChatScript's topics and rules are very similar and compatible with Oz's plans and goals.

* Godfrey will have an expressive face with which humans can identify. These eyes will open and close and track people in his room. His face will emote his feelings and in his final form his face and body will be designed to be artistically attractive. His inspirations are [Kismet](http://en.wikipedia.org/wiki/Kismet_%28robot%29) and the many robot's that have followed him.

* Godfrey should to be physically nimble in a domestic environment. I am defining nimble as being able to start, travel and stop across 15 feet (4.5 meters) in 4-5 seconds without damaging himself or his household. This makes complex demands on motors, propulsion, navigation, rapid decision making and balancing mechanisms. I believe Umashankar Nagarajan's [thesis](http://www.cs.cmu.edu/~unagaraj/Umashankar_PhD_Thesis.pdf) makes a very strong case that Godfrey needs to be a balancing robot if it wants to act realistically in a home environment. Godfrey will not be able to handle stairs or rough terrain.

* Godfrey should be of a height that humans feel comfortable interacting with and one they will take seriously as an independent agent. I will loosely define that to be 3.5 to 5 feet tall (1 to 1.5 meters). To the best of my knowledge there is no science defining an optimal height. However, my gut tells me if he is too small he will be perceived as a toy or a pet and if he is too large is will be scary and dangerous.

* Godfrey needs to avoid the [uncanny valley](http://en.wikipedia.org/wiki/Uncanny_valley). One way to avoid this valley is controlling Godfrey's external appearance. There are several possibilities to explore. The first is to make Godfrey look like a robot. For example [Serge](http://en.battlestarwiki.org/wiki/Serge), in Syfy's Caprica TV series, is of the approximate size and shape I am imagining for Godfrey ([here](http://lookpic.com/i/95/cKXQRCb.jpeg) is a better picture). Another option is to use puppet construction techniques to create an alien from another world (who for some strange reason decided to live in my home). Examples of this approach would be [Rygel XVI](http://farscape.wikia.com/wiki/Rygel_XVI), [Yoda](http://en.wikipedia.org/wiki/Yoda) or even like [Kermit the Frog](http://en.wikipedia.org/wiki/Kermit_the_Frog). Finally, a good solution might be to blend tech and natural looks like in this [Skyrim Dragon Priest Mask](http://www.instructables.com/id/Skyrim-Dragon-Priest-Mask/?ALLSTEPS).

* Godfrey should be relatively light so accidents, collisions or loss of balance don't cause too much damage to his home, its occupants or my marriage. I know of no objective way to decide the ideal weight so I am arbitrarily deciding on 15 to 20 lbs (7 to 9 kg).

* Godfrey will be self-contained. He will be able to go about his business and interact with family members without requiring the use of the Internet to access external data or computational resources. His WiFi will be primarily used to load software, monitor his performance and to send out alerts. Godfrey can optionally use the Internet to lookup reference material. For example, if I were to comment "I am going to London," Godfrey could look London up on the Internet and respond "That's a long trip. How long will it take you to get there?" However if the net is unavailable he might instead say "When do you leave?" My plan is to use [Freebase](http://www.freebase.com) and their REST API for reference queries. This will require some translations between Oz/ChatScript-style knowledge and its Freebase equivalent.

* Godfrey will be able to manage his power and recharge his batteries without human help. A custom docking station will be built for him to use.

* Building Godfrey will support the exploration of the open source and for-pay tools to design and build hardware. The technologies to be explored are electronic design, PC board manufacturing, CNC, laser cutting and 3D printing. These technologies have had explosive growth in the last few years and are ripe for exploitation in robot design. One website that has personally inspired me on these opportunities is Michal Zalewski's amazing [Guerrilla guide to CNC machining, mold making, and resin casting](http://lcamtuf.coredump.cx/gcnc/). It is a new world out there and building Godfrey will give me a chance to experiment with some of it.

* Godfrey should be simple to build and repair. This means trying to use parts, designs, and software that are reasonably simple and can be stable for long periods of time. I will attempt to use self contained hardware and software components connected by simple protocols and connectors. This is so easy to say and so very difficult to achieve. Maybe I will succeed.

* Ideally Godfrey that will cost less than $2000 to duplicate. Serious bonus points for a design that can be built under $1000. This is another goal that is simple to say and hard to achieve.


Project Plan
============
This project will take the approach of building a simple and crude end-to-end system and then iterating to replace the weaker components over time.


Phase I
=======
Phase I's goal is to build a simple robot that meets the basic requirements and get all the hardware and software functioning together.

The overall hardware architecture have a power train based on a two-wheel Segway-like balancing design controlled by an Arduino processor. This design only requires balancing in one axis while a [ballbot](http://en.wikipedia.org/wiki/Ballbot) design must balance in two axes. The face will be constructed using standard RC-servos and controlled by another Arduino. Both Arduinos will be attached to a Pandaboard for higher level control. I am hoping that a single Pandaboard will have enough computational power to support speech recognition, natural language processing, and run the reactive planner. How Godfrey will sense the environment for navigation and collision avoidance has not been decided. This version will have no docking port.

Its construction techniques will be wood, wood screws, hot glue, duct tape, etc.


Phase II
========
Phase II will take the learnings from Phase I and make Godfrey more realistic in all his weak dimensions. This means making the body and face be more artistically interesting. It will also require upgrades to the software components. This Phase will be based on a [Ballbot](http://en.wikipedia.org/wiki/Ballbot) motor train. This version will have a docking port and the necessary hardware and software to find, dock and charge itself.

This phase will explore building custom parts and frames using specifically designed PC boards, 3D printing, CNC, laser cutting, etc.
