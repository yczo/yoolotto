'''
contain all settings to control common importers for (DRAW'S and WINNNING'S)
game definition prototype:-

    "parent.code": {
            "game": "parent.name",
            "components": {
                "name": ["format", "identifier",True/False ,"state"] 
            },
            "common_states":[("SATE_REMOTE_ID","STATE_NAME"),
                             ("",""),
                             ]
        }
'''

ALL_GAMES = {
             
        #--------------------------------global game----------------------------------
        "Powerball": {
            "game": "Powerball",
            "components": {
                "Powerball": ["Powerball", "Powerball",True,"US"]
            },
            "common_states":[("TX","Texas")
                             #("NY","New York"),
                             #("AR","Arkansas"),
                             #("AZ","Arizona"),
                             #("CT","Connecticut"),
                             #("DC","Dist. of Columbia"),
                             #("DE","Delaware"),
                             #("FL","Florida"),
                             #("GA","Georgia"),
                             #("IA","Iowa"),
                             #("ID","Idaho"),
                             #("IL","Illinois"),
                             #("IN","Indiana"),
                             #("KS","Kansas"),
                             #("KY","Kentucky"),
                             #("LA","Louisiana"),
                             #("MA","Massachusetts"),
                             #("MD","Maryland"),
                             #("ME","Maine"),
                             #("MI","Michigan"),
                             #("MO","Missouri"),
                             #("NC","North Carolina"),
                             #("ND","North Dakota"),
                             #("NE","Nebraska"),
                             #("NH","New Hampshire"),
                             #("NJ","New Jersey"),
                             #("NM","New Mexico"),
                             #("OH","Ohio"),
                             #("OK","Oklahoma"),
                             #("OR","Oregon"),
                             #("PA","Pennsylvania"),
                             #("SC","South Carolina"),
                             #("SD","South Dakota"),
                             #("TN","Tennessee"),
                             #("VA","Virginia"),
                             #("VT","Vermont"),
                             #("WA","Washington"),
                             #("WI","Wisconsin"),
                             #("WV","West Virginia"),
                             #("RI","Rhode Island"),
                             #("MN","Minnesota"),
                             #("MT","Montana"),
                             #("CO","Colorado ")
                             #("",""),
                             ]
        },
        
        "MegaMillions": {
            "game": "Mega Millions",
            "components": {
                "MEGA Millions": ["MegaMillions", "MegaMillions",True,"US"],
                "Megaplier": ["Megaplier", "Megaplier",False,""]#single entry for all states
            },
            "common_states":[("TX","Texas")
                             #("NY","New York"),
                             #("AR","Arkansas"),
                             #("AZ","Arizona"),
                             #("CT","Connecticut"),
                             #("DC","Dist. of Columbia"),
                             #("DE","Delaware"),
                             #("FL","Florida"),
                             #("GA","Georgia"),
                             #("IA","Iowa"),
                             #("ID","Idaho"),
                             #("IL","Illinois"),
                             #("IN","Indiana"),
                             #("KS","Kansas"),
                             #("KY","Kentucky"),
                             #("LA","Louisiana"),
                             #("MA","Massachusetts"),
                             #("MD","Maryland"),
                             #("ME","Maine"),
                             #("MI","Michigan"),
                             #("MO","Missouri"),
                             #("NC","North Carolina"),
                             #("ND","North Dakota"),
                             #("NE","Nebraska"),
                             #("NH","New Hampshire"),
                             #("NJ","New Jersey"),
                             #("NM","New Mexico"),
                             #("OH","Ohio"),
                             #("OK","Oklahoma"),
                             #("OR","Oregon"),
                             #("PA","Pennsylvania"),
                             #("SC","South Carolina"),
                             #("SD","South Dakota"),
                             #("TN","Tennessee"),
                             #("VA","Virginia"),
                             #("VT","Vermont"),
                             #("WA","Washington"),
                             #("WI","Wisconsin"),
                             #("WV","West Virginia"),
                             #("RI","Rhode Island"),
                             #("MN","Minnesota"),
                             #("MT","Montana"),
                             #("CO","Colorado ")
                             #("",""),
                             ]
        },
        
        #-----------------------------CA------------------------------------
        
        "FantasyFive": {
            "game": "Fantasy Five",
            "components": {
                "Fantasy 5": ["FantasyFive", "FantasyFive",True,""]
            },
            "common_states":[("CA","California")]
        },
        "DailyFor": {
            "game": "Daily Four",
            "components": {
                "Daily 4": ["DailyFour", "DailyFour",True,""],
            },
            "common_states":[("CA","California")]
        },
        "DailyThree": {
            "game": "Daily Three",
            "components": {
                "Daily 3 Midday": ["DailyThree", "DailyThreeMidDay",True,""],
                "Daily 3 Evening": ["DailyThree", "DailyThreeEvening",True,""]
            },
            "common_states":[("CA","California")]
        },
        "DailyDerby": {
            "game": "Daily Derby",
            "components": {
                "Daily Derby": ["DailyDerby", "Daily Derby",True,""]
            },
            "common_states":[("CA","California")]
        },
        "SuperLottoPlus": {
            "game": "SuperLotto Plus",
            "components": {
                "SuperLotto Plus": ["SuperLottoPlus", "SuperLottoPlus",True,""]
            },
            "common_states":[("CA","California")]
        },
        "PowerballCA": {
            "game": "Powerball",
            "components": {
                "Powerball": ["Powerballca", "Powerballca",True,""]
            },
            "common_states":[("CA","California")]
        },
        "MegaMillionsCA": {
            "game": "MEGA Millions",
            "components": {
                "MEGA Millions": ["MegaMillionsCA", "MegaMillionsCA",True,""]
            },
            "common_states":[("CA","California")]
        },
             
        #----------------------------------------------------------TX----------------------------------
        "Lotto": {
            "game": "Lotto Texas",
            "components": {
                "Lotto": ["Lotto", "LottoTexas",True,""]
            },
            "common_states":[("TX","Texas")]
        },
        "DailyFour": {
            "game": "Daily Four",
            "components": {
                "Daily 4 Day": ["DailyFour", "DailyFourDay",True,""],
                "Daily 4 Night": ["DailyFour", "DailyFourNight",True,""],
                "Daily 4 Morning": ["DailyFour", "DailyFourMorning",True,""],
                "Daily 4 Evening": ["DailyFour", "DailyFourEvening",True,""]
            },
            "common_states":[("TX","Texas")]
        },
        "PickThree": {
            "game": "Pick Three",
            "components": {
                "Pick 3 Day": ["PickThree", "PickThreeDay",True,""],
                "Pick 3 Night": ["PickThree", "PickThreeNight",True,""],
                "Pick 3 Morning":  ["PickThree", "PickThreeMorning",True,""],
                "Pick 3 Evening": ["PickThree", "PickThreeEvening",True,""]
            },
            "common_states":[("TX","Texas")]
        },
        "CashFive": {
            "game": "Cash Five",
            "components": {
                "Cash 5": ["CashFive", "CashFive",True,""]
            },
            "common_states":[("TX","Texas")]
        },
        "TwoStep": {
            "game": "Two Step",
            "components": {
                "Two Step": ["TwoStep", "TwoStep",True,""]
            },
            "common_states":[("TX","Texas")]
        },
        "AllOrNothing": {
            "game": "All or Nothing",
            "components": {
                "All or Nothing Day": ["AllOrNothing", "AllOrNothingDay",True,""],
                "All or Nothing Night": ["AllOrNothing", "AllOrNothingNight",True,""],
                "All or Nothing Morning":  ["AllOrNothing", "AllOrNothingMorning",True,""],
                "All or Nothing Evening": ["AllOrNothing", "AllOrNothingEvening",True,""]
            },
            "common_states":[("TX","Texas")]
        }
        
    }

IMPORTER_ADMIN=('irfan@spa-systems.com')

