# Man-made objects. Format as in data_animals.py.
TEMPLATES = {
    "furn":  "thing house furniture gtbread wood hard",
    "kit":   "thing house kitchen hold",
    "tool":  "thing tool hold work metal hard",
    "elec":  "thing house electric buttons plastic",
    "veh":   "thing vehicle moves outdoor gtperson gtcar metal motor expensive wheels hard",
    "wear":  "thing wear cloth soft house hold",
    "bath":  "thing house bathroom hold",
    "office":"thing school hold work house",
    "toy":   "thing fun toy house hold",
    "sport": "thing fun hold outdoor",
    "mus":   "thing music fun sound hold expensive",
    "misc":  "thing house",
}

DATA = """
chair | furn | legs4
table | furn | legs4 kitchen
bed | furn | soft gtperson cloth
sofa | furn | soft gtperson cloth
wardrobe | furn | gtperson container
desk | furn | school work legs4
bookshelf | furn | container gtperson
armchair | furn | soft cloth
cupboard | furn | container kitchen
mirror | furn | glass bathroom shiny !wood
carpet | furn | cloth soft !hard !wood
bench | furn | outdoor long
fork | kit | metal sharp grey
knife | kit | metal sharp danger grey
spoon | kit | metal grey
plate | kit | round white hard
cup | kit | container hard
drinking glass | kit | glass container hard
bowl | kit | round container hard
frying pan | kit | metal hot black
kettle | kit | electric hot metal container water
toaster | kit | electric hot metal
microwave | kit | electric gtbread hot buttons !hold
oven | kit | electric hot gtbread !hold expensive
fridge | kit | electric cold gtperson container expensive white !hold
dishwasher | kit | electric clean water gtbread expensive !hold
blender | kit | electric sharp container
bottle | kit | glass plastic container
teapot | kit | container hot
chopsticks | kit | wood long
hammer | tool | wood
screwdriver | tool | sharp long
saw | tool | sharp danger
drill | tool | electric danger motor
axe | tool | sharp danger wood outdoor
shovel | tool | outdoor long garden
rake | tool | outdoor long garden
ladder | tool | gtperson !hold long
nail | tool | tiny sharp many grey
scissors | tool | sharp school
rope | tool | long cloth !metal !hard soft
bucket | tool | container plastic outdoor
lawn mower | tool | motor wheels outdoor gtbread !hold garden
torch | tool | light electric
broom | tool | clean long wood house
mop | tool | clean long water house
sponge | tool | clean soft water bathroom kitchen house !metal !hard
television | elec | screen gtbread expensive
computer | elec | screen expensive work school
laptop | elec | screen expensive work school hold
mobile phone | elec | screen communicate expensive hold
tablet | elec | screen expensive hold fun
camera | elec | expensive hold
radio | elec | sound music communicate
headphones | elec | music sound wear
loudspeaker | elec | sound music
remote control | elec | hold
games console | elec | fun toy expensive
wall clock | misc | round buildpart sound
calculator | elec | school hold screen work
light bulb | elec | light glass hot hold !buttons !plastic
lamp | elec | light !buttons
hair dryer | elec | bathroom hot hold
vacuum cleaner | elec | clean motor gtbread
washing machine | elec | clean water gtbread expensive cloth?
clothes iron | elec | hot metal hold
battery | elec | hold tiny metal !buttons !plastic
robot | elec | moves metal expensive toy
drone | elec | fly motor expensive fun
car | veh |
bus | veh | city long
lorry | veh | work
motorbike | veh | !gtcar fast danger
bicycle | veh | !gtcar !motor !expensive fun
train | veh | long city
aeroplane | veh | fly
helicopter | veh | fly sound
boat | veh | water swim !wheels
ship | veh | water swim sea !wheels
submarine | veh | water sea !wheels
canoe | veh | water swim !motor wood long !wheels !gtcar !metal
tractor | veh | farm work
fire engine | veh | red helps sound
ambulance | veh | white helps sound
taxi | veh | yellow city
rocket | veh | fly space !wheels
skateboard | veh | fun !motor wood !gtperson !gtcar !expensive hold
hot air balloon | veh | fly !wheels cloth colorful !metal
shirt | wear |
trousers | wear | long
jeans | wear | blue
dress | wear | colorful
jacket | wear | cold
coat | wear | cold
jumper | wear | cold
shoes | wear | ~cloth
boots | wear | ~cloth cold
sandals | wear | hot
socks | wear | many
hat | wear |
scarf | wear | long cold
gloves | wear | cold
tie | wear | long
belt | wear | long
swimsuit | wear | water
raincoat | wear | plastic water !cloth
backpack | wear | container school
handbag | wear | container jewel
umbrella | misc | water long metal hold
sunglasses | wear | glass plastic jewel !cloth hard
glasses | wear | glass !cloth hard
wristwatch | wear | jewel metal !cloth hard
ring | wear | jewel metal tiny round expensive !cloth hard shiny
necklace | wear | jewel metal !cloth shiny
earrings | wear | jewel tiny metal !cloth shiny
crown | wear | jewel metal expensive yellow !cloth hard shiny
toothbrush | bath | plastic long
toothpaste | bath | white soft
soap | bath | clean smell water
shampoo | bath | liquid clean smell
towel | bath | cloth soft
toilet | bath | white gtbread water !hold
bathtub | bath | water gtperson white !hold container
shower | bath | water !hold buildpart
comb | bath | plastic
razor | bath | sharp
toilet paper | bath | paper white soft
pen | office | write
pencil | office | write wood long
ruler | office | long plastic
notebook | office | paper write
book | office | read paper
newspaper | office | read paper
magazine | office | read paper colorful
envelope | office | paper communicate
postage stamp | office | paper tiny communicate
crayon | office | write colorful
chalk | office | write white
globe | office | round
map | office | paper read
ball | toy | round outdoor
football | sport | round
tennis racket | sport | long
golf club | sport | long metal expensive
baseball bat | sport | wood long
skis | sport | long cold !hold
kite | sport | fly colorful
doll | toy |
teddy bear | toy | soft cloth brown
dice | toy | tiny
playing cards | toy | paper many
chess set | toy | wood
building blocks | toy | plastic many colorful
balloon | toy | round colorful fly
frisbee | sport | round plastic fly
trampoline | sport | jump gtperson !hold
marbles | toy | glass round tiny many
toy car | toy | wheels
fishing rod | sport | long water
tent | sport | cloth gtperson !hold
surfboard | sport | water swim long !hold
guitar | mus | wood
piano | mus | gtperson wood !hold buttons
violin | mus | wood
drum | mus | round
trumpet | mus | metal
flute | mus | long metal
harmonica | mus | metal tiny !expensive
harp | mus | gtperson wood !hold
microphone | mus | electric !expensive
bell | misc | metal sound
key | misc | metal tiny hold
door | misc | buildpart wood gtperson
window | misc | buildpart glass
stairs | misc | buildpart !hold gtperson
candle | misc | light hot smell hold
match | misc | hot tiny wood danger hold
lighter | misc | danger hot hold
coin | misc | metal round tiny many hold
banknote | misc | paper hold
credit card | misc | plastic hold
cardboard box | misc | paper container
plastic bag | misc | plastic container
basket | misc | wood container
pillow | misc | soft cloth
blanket | misc | soft cloth
vase | misc | glass container
flag | misc | cloth colorful outdoor
gun | misc | weapon metal danger hold
sword | misc | weapon metal sharp danger long
bow and arrow | misc | weapon wood sharp danger
rubbish bin | misc | container plastic
flowerpot | misc | container garden
needle | misc | sharp metal tiny
magnet | misc | metal hold
telescope | misc | glass long expensive
binoculars | misc | glass expensive hold
compass | misc | round hold
anchor | misc | metal water sea
wheel | misc | round
"""
