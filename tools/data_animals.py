# Animals. Line format:  name | template | tokens
# token "x" = yes, "-x" = definitely no, "~x" = unknown/depends, "!x" = drop from template.
# Anything not mentioned is a weak "no".
TEMPLATES = {
    "pet":  "animal alive natural old moves mammal fur tail legs4 pet house sound gtbread",
    "wildm":"animal alive natural old moves outdoor wild mammal fur tail legs4",
    "farm": "animal alive natural old moves outdoor mammal fur tail legs4 farm sound gtbread many",
    "bird": "animal alive natural old moves outdoor wild bird feathers fly eggs tail sound",
    "fish": "animal alive natural old moves wild fish scales swim water nolegs eggs tail",
    "rept": "animal alive natural old moves outdoor wild reptile scales eggs tail",
    "amph": "animal alive natural old moves outdoor wild amphibian water swim eggs",
    "ins":  "animal alive natural old moves outdoor wild insect manylegs tiny many eggs hold",
    "bug":  "animal alive natural old moves outdoor wild bug",
    "seam": "animal alive natural old moves wild mammal water swim sea nolegs tail",
}

DATA = """
dog | pet | meat brown black white europe ~danger fast farm city
cat | pet | meat night climb jump europe ~pattern city ~gtbread
hamster | pet | -gtbread hold brown soft tiny
guinea pig | pet | ~gtbread hold soft brown white -tail -sound
rabbit | pet | jump soft white brown outdoor farm europe ~eat wild garden
ferret | pet | meat long white brown
horse | farm | ride fast brown black white gtperson ~eat
pony | farm | ride brown white -gtperson pet
donkey | farm | ride grey brown -fast
cow | farm | eat black white brown gtperson horns
bull | farm | black brown gtperson horns danger -many
pig | farm | eat pink ~fur smell
sheep | farm | eat white soft
goat | farm | eat horns white brown climb jump
llama | farm | white brown gtperson -europe
alpaca | farm | white brown soft -europe
yak | farm | horns brown black polar -europe
camel | wildm | farm ride dry africa gtperson brown
lion | wildm | meat africa danger gtperson sound yellow brown many fast
tiger | wildm | meat danger gtperson pattern orange black forest swim fast tropical
leopard | wildm | meat danger pattern yellow africa climb night fast
cheetah | wildm | meat pattern yellow africa fast danger
jaguar | wildm | meat pattern yellow forest tropical danger swim
lynx | wildm | meat europe forest brown pattern
cougar | wildm | meat brown climb danger forest
elephant | wildm | gtcar grey africa tropical many sound horns !fur
giraffe | wildm | gtcar gtperson pattern yellow brown africa many long
zebra | wildm | pattern black white africa many fast gtperson
hippopotamus | wildm | gtcar grey water swim africa danger !fur
rhinoceros | wildm | horns grey africa danger gtcar !fur
gorilla | wildm | !tail !legs4 black africa forest gtperson climb
chimpanzee | wildm | !tail !legs4 black brown africa forest climb sound
orangutan | wildm | !tail !legs4 orange forest tropical climb
monkey | wildm | !legs4 climb forest tropical brown sound jump many
baboon | wildm | !legs4 africa brown many sound
lemur | wildm | climb forest tropical pattern grey night jump
sloth | wildm | climb forest tropical brown -fast
brown bear | wildm | meat danger brown gtperson forest europe climb swim
polar bear | wildm | meat danger white polar gtperson swim sea
panda | wildm | black white forest -meat gtperson climb
red panda | wildm | red orange forest climb
koala | wildm | australia climb grey forest !tail soft
kangaroo | wildm | australia jump brown gtperson ~legs4 fast
wombat | wildm | australia brown night under
platypus | wildm | australia water swim eggs brown venom
wolf | wildm | meat danger grey forest europe many night sound
fox | wildm | meat orange europe forest night city
coyote | wildm | meat brown grey night sound
jackal | wildm | meat brown africa night
hyena | wildm | meat africa pattern sound night danger
deer | wildm | horns brown forest europe fast eat jump many
moose | wildm | horns brown forest europe gtperson polar
reindeer | wildm | horns brown polar many farm ~ride
squirrel | wildm | climb forest europe brown red -gtbread jump city garden
chipmunk | wildm | climb forest brown pattern -gtbread hold
hedgehog | wildm | sharp brown europe night -gtbread garden
badger | wildm | black white europe night forest under
otter | wildm | water swim brown europe meat
beaver | wildm | water swim brown forest
raccoon | wildm | night pattern grey city climb
skunk | wildm | smell black white night
bat | wildm | fly night black brown !legs4 hold -gtbread many
mole | wildm | black under -gtbread europe garden
hare | wildm | jump fast brown europe eat
bison | wildm | horns brown gtperson many
buffalo | wildm | horns black africa gtperson many farm
warthog | wildm | horns africa brown grey
antelope | wildm | horns africa fast brown many jump
gazelle | wildm | horns africa fast brown many jump -gtperson
anteater | wildm | long forest tropical brown
armadillo | wildm | shell brown night
porcupine | wildm | sharp night brown
weasel | wildm | meat long brown europe -gtbread
meerkat | wildm | africa brown many dry under
mongoose | wildm | meat africa brown long
tapir | wildm | forest tropical black white
capybara | wildm | water swim brown tropical
opossum | wildm | night grey climb
mammoth | wildm | extinct gtcar brown polar horns -alive -moves
sabre-toothed tiger | wildm | extinct meat danger brown sharp -alive -moves
seal | seam | grey polar europe meat
walrus | seam | brown polar gtperson horns
sea lion | seam | brown sound meat
dolphin | seam | grey meat fast jump sound many
whale | seam | gtcar blue grey sound
orca | seam | black white meat danger gtcar
narwhal | seam | horns grey polar
manatee | seam | grey tropical gtperson
eagle | bird | meat brown danger europe
hawk | bird | meat brown fast
falcon | bird | meat brown grey fast
owl | bird | meat night brown forest europe
penguin | bird | !fly swim water sea polar black white many
ostrich | bird | !fly africa fast gtperson black ~eat
emu | bird | !fly australia brown gtperson fast
flamingo | bird | water pink africa tropical many long
peacock | bird | colorful blue green farm
swan | bird | water swim white europe gtbread
pigeon | bird | grey city many europe
sparrow | bird | brown -gtbread city europe many hold
crow | bird | black europe city
raven | bird | black europe forest
robin | bird | red brown europe hold garden
blackbird | bird | black europe hold garden
magpie | bird | black white europe city
seagull | bird | white grey sea water europe many
woodpecker | bird | forest europe red black climb
hummingbird | bird | tiny colorful tropical fast green hold
parrot | bird | pet house colorful green red tropical forest climb
budgie | bird | pet house green blue yellow hold australia
canary | bird | pet house yellow hold
kiwi bird | bird | !fly night brown forest
toucan | bird | tropical forest black colorful orange
pelican | bird | water white sea gtbread meat
stork | bird | white water europe long
heron | bird | water grey long europe meat
vulture | bird | meat africa black brown
puffin | bird | sea black white orange europe polar swim
kingfisher | bird | blue orange water europe
cuckoo | bird | grey europe forest
chicken | bird | farm eat ~fly !wild many europe white brown red gtbread
rooster | bird | farm ~fly !wild red colorful europe gtbread
duck | bird | water swim farm eat europe many
goose | bird | water swim farm eat white europe gtbread many
turkey | bird | farm eat !fly gtbread brown
dodo | bird | extinct !fly -alive -moves grey
shark | fish | sea meat danger grey gtperson fast ~scales
salmon | fish | eat pink orange europe sea
tuna | fish | eat sea gtbread grey fast
cod | fish | eat sea europe grey gtbread
trout | fish | eat europe pattern
herring | fish | eat sea europe many grey
eel | fish | long europe eat ~scales
carp | fish | europe eat brown
piranha | fish | meat danger tropical many
clownfish | fish | orange white sea tropical pattern hold
goldfish | fish | pet house orange !wild hold
swordfish | fish | sea sharp gtperson
stingray | fish | sea venom grey
seahorse | fish | sea tiny tropical yellow hold
pufferfish | fish | sea venom round tropical
catfish | fish | eat brown
sardine | fish | eat sea many grey hold
jellyfish | bug | sea water swim venom round many soft
octopus | bug | sea water swim eat meat manylegs soft
squid | bug | sea water swim eat manylegs long
crab | bug | sea water shell manylegs eat red hold
lobster | bug | sea water shell manylegs eat red gtbread
shrimp | bug | sea water swim eat pink many hold tiny
starfish | bug | sea water orange manylegs hold
snail | bug | shell europe garden hold tiny ~eat
slug | bug | europe soft brown garden hold
earthworm | bug | long soft brown pink nolegs many europe under garden
spider | bug | manylegs black venom ~danger house europe many tiny
tarantula | bug | manylegs black brown venom tropical hold
scorpion | bug | manylegs venom danger dry tropical tail
centipede | bug | manylegs long venom tiny
oyster | bug | sea shell eat -moves hold
mussel | bug | sea shell eat black many hold
coral | bug | sea tropical colorful hard many -moves
sea urchin | bug | sea sharp round black hold
ant | ins | black red
bee | ins | fly venom yellow black pattern sound
wasp | ins | fly venom yellow black pattern danger
hornet | ins | fly venom yellow brown danger
butterfly | ins | fly colorful !many
moth | ins | fly night brown house
housefly | ins | fly black house sound
mosquito | ins | fly venom danger tropical sound night
ladybird | ins | fly red black pattern round garden
beetle | ins | black hard fly
grasshopper | ins | green jump sound
cricket | ins | jump sound night brown
dragonfly | ins | fly water long colorful blue fast
cockroach | ins | brown house night fast
termite | ins | tropical white
flea | ins | jump black meat pet
caterpillar | ins | green long soft garden
firefly | ins | fly night light
praying mantis | ins | green meat long
snake | rept | nolegs long venom danger green
python | rept | nolegs long gtperson danger tropical meat pattern
cobra | rept | nolegs long venom danger tropical
rattlesnake | rept | nolegs long venom danger dry sound
crocodile | rept | legs4 water swim danger meat gtperson green tropical
alligator | rept | legs4 water swim danger meat gtperson green
lizard | rept | legs4 green brown hold climb fast
chameleon | rept | legs4 green colorful climb tropical hold
gecko | rept | legs4 climb tropical hold night
iguana | rept | legs4 green tropical climb
tortoise | rept | legs4 shell brown green -fast ~pet
turtle | rept | legs4 shell water swim sea green
komodo dragon | rept | legs4 danger gtperson meat tropical
tyrannosaurus | rept | extinct gtcar danger meat -alive -moves
triceratops | rept | extinct gtcar horns legs4 -alive -moves
frog | amph | green jump legs4 sound hold europe
toad | amph | brown legs4 jump europe hold venom garden
salamander | amph | legs4 tail pattern black yellow
newt | amph | legs4 tail europe hold
axolotl | amph | legs4 tail pink white pet
"""
