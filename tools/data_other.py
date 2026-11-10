# "Other": food, drinks, places, buildings, sky & space, materials, body, people.
TEMPLATES = {
    "food":  "other eat house kitchen hold",
    "meal":  "other eat meal house kitchen cooked hot",
    "drink": "other liquid drink house kitchen ~eat",
    "place": "other place outdoor gtcar",
    "nplace":"other place outdoor gtcar natural old",
    "bldg":  "other building gtcar city place hard ~thing",
    "sky":   "other natural sky outdoor old",
    "mat":   "other material hard",
    "body":  "other body alive soft ~animal",
    "pers":  "other person alive moves gtbread sound ~animal",
}

DATA = """
bread | food | baked brown soft ~plant
cheese | food | dairy yellow smell ~animal
butter | food | dairy yellow soft ~animal
yogurt | food | dairy white soft cold ~animal
egg | food | white round hard cooked ~animal shell
ice cream | food | dairy cold sweet snack soft ~animal
pizza | meal | baked round
hamburger | meal | hasmeat
hot dog | meal | hasmeat long
sandwich | meal | !cooked !hot hold
spaghetti | meal | long
soup | meal | liquid
salad | meal | green !cooked !hot cold ~plant
sushi | meal | hasmeat cold !cooked !hot hold
steak | meal | hasmeat ~animal
sausage | food | hasmeat long cooked ~animal
bacon | food | hasmeat cooked pink ~animal
ham | food | hasmeat pink ~animal
french fries | food | hot yellow long snack cooked ~plant
crisps | food | snack yellow ~plant
popcorn | food | snack white many ~plant
chocolate | food | snack sweet brown ~plant
cake | food | baked sweet snack soft
birthday cake | food | baked sweet snack colorful soft !hold fun
cookie | food | baked sweet snack round hard
pancake | food | round hot sweet cooked
doughnut | food | round sweet baked snack
apple pie | food | baked hot sweet ~plant
sweets | food | sweet colorful tiny snack many
lollipop | food | sweet round snack colorful
chewing gum | food | sweet snack tiny
marshmallow | food | white soft sweet snack pink
honey | food | sweet yellow liquid ~animal
jam | food | sweet red ~plant
sugar | food | sweet white tiny many material ~plant
salt | food | white tiny many material natural
flour | food | white many material ~plant
taco | meal | hasmeat hold
curry | meal | smell yellow
omelette | meal | yellow ~animal
breakfast cereal | food | sweet many ~plant
noodles | meal | long
meatballs | meal | hasmeat round
lasagne | meal | baked hasmeat
ketchup | food | red liquid sweet ~plant
mustard | food | yellow smell
peanut butter | food | brown sweet soft ~plant
water | drink | natural outdoor old ~eat
coffee | drink | hot brown smell ~plant
tea | drink | hot ~plant
hot chocolate | drink | hot sweet brown dairy
orange juice | drink | orange sweet cold ~plant
lemonade | drink | sweet yellow cold sour
cola | drink | brown sweet cold
beer | drink | alcohol yellow cold ~plant
wine | drink | alcohol red ~plant
champagne | drink | alcohol expensive
milk | drink | dairy white cold ~animal
milkshake | drink | dairy sweet cold snack
beach | nplace | water sea yellow hot
mountain | nplace | cold grey
forest | nplace | green forest
desert | nplace | hot dry yellow
jungle | nplace | tropical green forest hot
island | nplace | water sea
volcano | nplace | danger hot
lake | nplace | water blue
river | nplace | water long
ocean | nplace | water sea blue
waterfall | nplace | water sound
cave | nplace | black
glacier | nplace | cold white polar water
meadow | nplace | green
swamp | nplace | water smell green
north pole | nplace | cold white polar
canyon | nplace | dry
park | place | city green
city | place | city
village | place |
garden | place | house green
playground | place | city fun
zoo | place | city fun animal?
road | place | city long hard
airport | place | city building
harbour | place | water sea city
school | bldg | school
hospital | bldg | helps
church | bldg | old
castle | bldg | old !city
tower | bldg | long
skyscraper | bldg | glass long
lighthouse | bldg | light sea water
barn | bldg | farm wood !city
supermarket | bldg | eat
restaurant | bldg | eat
library | bldg | read
museum | bldg | old
cinema | bldg | fun screen
stadium | bldg | fun
prison | bldg | danger
bridge | bldg | water long !building
windmill | bldg | wood old !city
pyramid | bldg | old dry hot yellow !city
igloo | bldg | cold white polar !city
factory | bldg | work smell
house | bldg | house
fire station | bldg | red helps
bank | bldg | expensive
sun | sky | hot yellow round space light gtcar
moon | sky | white round space light night gtcar
star | sky | space light many night gtcar
planet | sky | round space gtcar
earth | sky | round space blue green gtcar
mars | sky | red round space gtcar
comet | sky | space light gtcar
black hole | sky | space black danger gtcar
cloud | sky | white soft water gtcar many
rain | sky | water liquid many cold
snow | sky | white cold soft many
lightning | sky | light danger hot electric
thunder | sky | sound danger gas
rainbow | sky | colorful gas
wind | sky | gas cold sound
tornado | sky | danger gas
fog | sky | white gas water grey
ice | mat | cold hard water white natural
fire | mat | hot danger light red orange gas natural
smoke | mat | grey gas
air | mat | gas natural !hard
sky | sky | blue gas
rock | mat | grey hard outdoor natural old
sand | mat | yellow many tiny outdoor water natural
gold | mat | yellow metal expensive shiny natural
silver | mat | grey metal shiny expensive natural
iron | mat | metal grey natural
copper | mat | metal orange natural
diamond | mat | expensive shiny tiny jewel natural
coal | mat | black natural
oil | mat | liquid black danger natural !hard
mud | mat | brown soft water natural !hard
clay | mat | soft brown natural !hard
soil | mat | brown outdoor many natural garden !hard
plastic | mat | plastic !hard
glass | mat | glass
paper | mat | paper white !hard
wood | mat | wood brown ~plant
concrete | mat | grey city
cotton | mat | white soft cloth ~plant !hard
wool | mat | white soft cloth ~animal !hard
leather | mat | brown wear ~animal
pearl | mat | white round shiny expensive sea tiny jewel natural ~animal
hand | body | hold
foot | body |
head | body | round
eye | body | round tiny
ear | body |
nose | body | smell
mouth | body |
tooth | body | white hard tiny !soft
tongue | body | pink soft
hair | body | long many
heart | body | red
brain | body | pink soft
bone | body | white hard !soft
skin | body |
finger | body | long hold
knee | body |
arm | body | long
leg | body | long
blood | body | red liquid
fingernail | body | hard tiny !soft
doctor | pers | helps white
nurse | pers | helps uniform
teacher | pers | school helps
police officer | pers | helps uniform danger city
firefighter | pers | helps uniform danger red
soldier | pers | uniform danger weapon
pilot | pers | fly uniform
farmer | pers | outdoor farm work
chef | pers | kitchen uniform white
baker | pers | baked kitchen
astronaut | pers | space uniform expensive
king | pers | jewel old expensive
queen | pers | jewel old expensive
baby | pers | soft family !gtperson tiny?
mother | pers | family
father | pers | family
clown | pers | fun colorful
magician | pers | fun
pirate | pers | danger sea old weapon
knight | pers | old weapon metal
cowboy | pers | ride
dentist | pers | helps
postman | pers | communicate uniform
scientist | pers | work school
singer | pers | music
football player | pers | fun outdoor
mechanic | pers | tool work
lifeguard | pers | water helps
santa claus | pers | red white fun old
"""
