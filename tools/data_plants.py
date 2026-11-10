# Plant kingdom (incl. fungi). Format as in data_animals.py.
TEMPLATES = {
    "fruit": "plant natural old eat fruit sweet juicy hold ~alive kitchen house",
    "veg":   "plant natural old eat veg hold ~alive garden kitchen house",
    "tree":  "plant alive natural old outdoor tree gtcar wood hard green",
    "flower":"plant alive natural old outdoor flower garden smell hold",
    "herb":  "plant natural old eat herb kitchen smell house hold",
    "seed":  "plant natural old eat seed tiny hard many hold kitchen house",
    "wild":  "plant alive natural old outdoor green",
    "fung":  "plant natural old outdoor fungus forest soft ~alive",
}

DATA = """
apple | fruit | red green round europe garden
pear | fruit | green yellow europe garden
banana | fruit | yellow long peel tropical !juicy soft
orange | fruit | orange round peel tropical drink
lemon | fruit | yellow sour !sweet drink tropical
lime | fruit | green sour !sweet tropical tiny
grapefruit | fruit | yellow pink sour round peel tropical
mandarin | fruit | orange round peel tiny
grape | fruit | green purple tiny bush many drink alcohol europe round
strawberry | fruit | red garden europe tiny sharp?
raspberry | fruit | red bush tiny europe garden
blueberry | fruit | blue tiny bush europe forest many round
blackberry | fruit | black bush europe tiny
cherry | fruit | red stone tiny europe round tree?
plum | fruit | purple stone europe round garden
peach | fruit | stone orange pink soft round
apricot | fruit | stone orange europe
mango | fruit | stone orange yellow tropical
pineapple | fruit | yellow tropical sharp peel gtbread
coconut | fruit | brown hard round tropical drink !juicy shell
kiwi fruit | fruit | brown green peel sour
watermelon | fruit | green red gtbread round
melon | fruit | yellow green round gtbread
papaya | fruit | orange tropical
passion fruit | fruit | purple tropical sour round
pomegranate | fruit | red round many
fig | fruit | purple soft
avocado | fruit | green stone !sweet tropical soft
date | fruit | brown dry stone !juicy tropical
olive | fruit | green black stone !sweet !juicy europe tiny
tomato | fruit | red round !sweet garden ~veg
cranberry | fruit | red sour tiny bush drink
gooseberry | fruit | green sour bush tiny garden
blackcurrant | fruit | black sour bush tiny drink garden
lychee | fruit | white red peel tropical stone tiny
dragon fruit | fruit | pink tropical white
guava | fruit | green pink tropical
elderberry | fruit | black tiny bush drink europe
plantain | fruit | green long peel tropical cooked !sweet
jackfruit | fruit | green gtbread tropical !hold
durian | fruit | smell sharp tropical gtbread yellow
potato | veg | under brown cooked peel round
sweet potato | veg | under orange cooked sweet peel
carrot | veg | orange under long sweet
onion | veg | under smell peel round white
garlic | veg | under smell white tiny herb
leek | veg | long green white cooked
cabbage | veg | green round leafy cooked gtbread
lettuce | veg | green leafy juicy
spinach | veg | green leafy cooked
kale | veg | green leafy cooked
broccoli | veg | green cooked
cauliflower | veg | white cooked
brussels sprout | veg | green round tiny cooked
cucumber | veg | green long juicy
bell pepper | veg | red green yellow juicy
chilli pepper | veg | red hot long danger tiny tropical
aubergine | veg | purple cooked
courgette | veg | green long cooked
pumpkin | veg | orange round gtbread cooked !hold
pea | veg | green tiny seed round many cooked
green bean | veg | green long cooked seed
sweetcorn | veg | yellow seed many cooked sweet long
beetroot | veg | red purple under round cooked
radish | veg | red round under tiny
turnip | veg | white purple under round cooked
parsnip | veg | white under long cooked
celery | veg | green long juicy
asparagus | veg | green long cooked expensive?
artichoke | veg | green cooked
rhubarb | veg | red long sour cooked
okra | veg | green long tropical cooked
fennel | veg | white green smell
watercress | veg | green leafy water
cassava | veg | under brown cooked tropical peel
oak | tree | europe forest brown seed?
beech | tree | europe forest
birch | tree | white europe forest
pine | tree | evergreen sharp europe forest smell
spruce | tree | evergreen sharp europe forest
christmas tree | tree | evergreen sharp house colorful light
maple | tree | red europe forest sweet drink
willow | tree | water europe long
palm tree | tree | tropical !wood long
chestnut tree | tree | europe brown
poplar | tree | europe long
linden | tree | europe smell
yew | tree | evergreen danger europe
cedar | tree | evergreen smell
cypress | tree | evergreen europe long
redwood | tree | evergreen red forest
baobab | tree | africa dry
eucalyptus | tree | australia smell evergreen
bonsai | tree | house hold !gtcar !outdoor expensive
mahogany | tree | tropical brown expensive forest
ebony | tree | black tropical expensive
hazel | tree | europe bush seed
holly | tree | evergreen red sharp europe
juniper | tree | evergreen bush europe smell
larch | tree | europe forest
rowan | tree | red europe
acacia | tree | africa dry sharp
mangrove | tree | water tropical sea
rubber tree | tree | tropical forest
rose | flower | red sharp europe colorful bush
tulip | flower | red yellow colorful europe
daisy | flower | white yellow europe tiny many
sunflower | flower | yellow gtperson seed
lily | flower | white
orchid | flower | tropical colorful expensive house
daffodil | flower | yellow europe
dandelion | flower | yellow europe tiny many ~eat
poppy | flower | red europe
lavender | flower | purple europe herb bush
lotus | flower | water pink white
violet | flower | purple tiny europe
carnation | flower | pink red
chrysanthemum | flower | colorful yellow
marigold | flower | orange yellow
iris | flower | purple blue
hyacinth | flower | blue purple
jasmine | flower | white drink tropical bush
lilac | flower | purple bush europe
hibiscus | flower | red tropical drink
forget-me-not | flower | blue tiny europe
snowdrop | flower | white cold europe tiny
peony | flower | pink
geranium | flower | red house
dahlia | flower | colorful
water lily | flower | water white pink swim
bluebell | flower | blue forest europe
magnolia | flower | pink white tree
hydrangea | flower | blue pink bush
poinsettia | flower | red house
foxglove | flower | danger purple europe
thistle | flower | sharp purple europe
heather | flower | purple europe bush
edelweiss | flower | white cold
basil | herb | green
parsley | herb | green
mint | herb | green drink garden
rosemary | herb | green evergreen bush
thyme | herb | green tiny
oregano | herb | green
dill | herb | green
chives | herb | green long garden
coriander | herb | green
sage | herb | green grey
bay leaf | herb | green
cinnamon | herb | brown hard sweet long
black pepper | herb | black tiny seed many
vanilla | herb | black sweet expensive tropical long
saffron | herb | red expensive
ginger | herb | under brown hot drink
turmeric | herb | yellow orange under
nutmeg | herb | brown round hard
cloves | herb | brown tiny many
paprika | herb | red
chamomile | herb | flower white yellow drink
peanut | seed | brown under shell
walnut | seed | brown shell
hazelnut | seed | brown round shell
almond | seed | brown
cashew | seed | tropical white
pistachio | seed | green shell
chestnut | seed | brown cooked shell
coffee bean | seed | brown drink tropical
cocoa bean | seed | brown drink tropical
rice | seed | white cooked
wheat | seed | yellow outdoor long
oats | seed | cooked
barley | seed | drink alcohol
sesame | seed | white
lentil | seed | cooked
chickpea | seed | cooked round
soybean | seed | cooked
kidney bean | seed | red cooked
hops | seed | drink alcohol green !hard
grass | wild | many europe
moss | wild | soft tiny water forest
fern | wild | forest
cactus | wild | sharp dry tropical house
ivy | wild | europe evergreen climb bush
bamboo | wild | long tropical gtperson ~eat hard wood
seaweed | wild | water sea eat swim
algae | wild | water tiny many
reed | wild | water long
nettle | wild | sharp danger europe ~eat
venus flytrap | wild | meat house hold
aloe vera | wild | house sharp dry
sugar cane | wild | long sweet tropical drink
tobacco | wild | smell danger
cotton plant | wild | white soft cloth
mistletoe | wild | white evergreen
poison ivy | wild | danger
mushroom | fung | eat brown white cooked
toadstool | fung | red white pattern danger
truffle | fung | eat expensive under smell black
mould | fung | green many house
chanterelle | fung | yellow eat europe
"""
