from yoolotto.lottery.models import LotteryDraw, LotteryGameComponent,LotteryCountryDivision



class invalidExcelField(Exception):
    def __init__(self,msg=None,value=None,row=None):
        self.value=value
        self.msg=msg
        self.row=row
    
    def error_msg(self):
        return self.msg if self.msg else "Invalid data="+str(self.value)+" for excel row="+str(self.row)+" !"
    
#      
#   
# try:
#     raise invalidExcelField(12,1,"Need")
#     print "done"
# except invalidExcelField,e:
#     print e.error_msg()
#     raise RuntimeError("stop prcessing")


def removeDuplicate(date,component,division):
    draws=LotteryDraw.objects.filter(component=component,date=date,division=division)
    if draws.count() > 1:
        print "    duplicate rows,expected 1 found :"+str(draws.count())+" for "
        print "    date :"+str(date)
        print "    component :"+str(component)
        print "    division :"+str(division)
        for d in draws[1:]:#skipped first row and delete all
            id=d.id
            d.delete()
            print "        deleted,row id ="+str(id)
            
            
    
def take_dump(path,shortcut,sheet):
    import xlrd 
    import datetime
    workbook = xlrd.open_workbook(path)
    worksheet = workbook.sheet_by_index(sheet)
    
    xlrd_type={0:'empty',1:'text',2:'number',3:'date',4:'bool',5:'error code',6:''}
    game_shortcuts={
                          1:"TX_CASH_OF_FIVE",
                          2:"TX_ALL_OR_NOTHING",
                          3:"TX_LOTTO_TEXAS",
                          4:"TX_TEXAS_TWO_STEP",
                          
                          5:"CA_MEGAMILLION",
                          6:"CA_POWERBALL",
                          7:"CA_SUPER_LOTTO_PLUS",
                          8:"CA_FANTASY5",
                          9:"CA_DAILY4",
                          10:"CA_DAILY3",
                          11:"CA_DAILY_DERBY",
                          12:"TX_POWERBALL",
                          13:"TX_MEGAMILLION"
                          }
        
        
        
    game=game_shortcuts[shortcut]#import for game
    print "FILLING FOR : "+game_shortcuts[shortcut]
    print "-"*50
    
    for r in range(1,worksheet.nrows):
        if game=="TX_MEGAMILLION":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            component=LotteryGameComponent.objects.get(remote_id="113",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            numbers=worksheet.cell_value(r,1).replace(" ","").replace("-",",")
             
            try:
                if len(numbers.split(","))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+numbers+"]"
            
            jackpot=worksheet.cell_value(r,2)
            five_of_five_only=worksheet.cell_value(r,3)  
            four_of_five_megaball =worksheet.cell_value(r,4)
            four_of_five_only=worksheet.cell_value(r,5)
            three_of_five_with_megaball =worksheet.cell_value(r,6)
            three_of_five_only=worksheet.cell_value(r,7)
            two_of_five_megaball =worksheet.cell_value(r,8)
            one_of_five_megaball =worksheet.cell_value(r,9)
            megaball =worksheet.cell_value(r,10)
            
            #megaplier = 
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.jackpot=jackpot
            _draw.five_of_five_only=five_of_five_only
            _draw.four_of_five_megaball =four_of_five_megaball
            _draw.four_of_five_only=four_of_five_only
            _draw.three_of_five_with_megaball =three_of_five_with_megaball
            _draw.three_of_five_only=three_of_five_only
            _draw.two_of_five_megaball =two_of_five_megaball
            _draw.one_of_five_megaball =one_of_five_megaball
            _draw.megaball =megaball
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="TX_POWERBALL":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            component=LotteryGameComponent.objects.get(remote_id="101",division=division,name="Powerball")#name is necessary otherwise return two rows b/c texas has two games(powerball,powerplay) who's remote id is same
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            
            numbers=worksheet.cell_value(r,1).replace(" ","").replace("-",",")
             
            try:
                if len(numbers.split(","))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+numbers+"]"
            
            #result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            #powerplay = 
            jackpot =worksheet.cell_value(r,2)
            five_of_five_only = worksheet.cell_value(r,3)
            four_of_five_powerball = worksheet.cell_value(r,4)
            four_of_five_only = worksheet.cell_value(r,5)
            three_of_five_with_powerball = worksheet.cell_value(r,6)
            three_of_five_only = worksheet.cell_value(r,7)
            two_of_five_powerball = worksheet.cell_value(r,8)
            one_of_five_powerball =worksheet.cell_value(r,9)
            powerball_only =worksheet.cell_value(r,10)
            
            #two_of_five_only = 
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.jackpot = jackpot
            _draw.five_of_five_only = five_of_five_only
            _draw.four_of_five_powerball = four_of_five_powerball
            _draw.four_of_five_only = four_of_five_only
            _draw.three_of_five_with_powerball = three_of_five_with_powerball
            _draw.three_of_five_only = three_of_five_only
            _draw.two_of_five_powerball = two_of_five_powerball
            _draw.one_of_five_powerball =one_of_five_powerball
            _draw.powerball_only =powerball_only
            _draw.result=result
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_DAILY_DERBY":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="CA3",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            result_with_race_time=worksheet.cell_value(r,1).replace(" ","").split("-")
            
            
             
            try:
                if len(result_with_race_time)!=4:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            
            
            result="["+(",".join(result_with_race_time[0:3]))+"]"
            race_time='''"'''+result_with_race_time[3][1:8]+'''"'''
            
#             print result_with_race_time
#             print result
#             print race_time
            
            jackpot= worksheet.cell_value(r,2)
            exacta_with_racetime=worksheet.cell_value(r,3)
            win_with_racetime=worksheet.cell_value(r,4)
            race_time_amount=worksheet.cell_value(r,5)
            trifecta=worksheet.cell_value(r,6)
            exacta=worksheet.cell_value(r,7)
            win=worksheet.cell_value(r,8)
            
            
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            _draw.race_time=race_time
            
            _draw.jackpot= jackpot
            _draw.exacta_with_racetime=exacta_with_racetime
            _draw.win_with_racetime=win_with_racetime
            _draw.race_time_amount=race_time_amount
            _draw.trifecta=trifecta
            _draw.exacta=exacta
            _draw.win=win
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_DAILY3":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            possible_components={
                                 'midday':LotteryGameComponent.objects.get(remote_id="CAC",division=division),
                                 'evening':LotteryGameComponent.objects.get(remote_id="CAA",division=division),
                                 }
            
            component=possible_components[worksheet.cell_value(r,1).lower().strip()]
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
             
            try:
                if len(worksheet.cell_value(r,2).replace(" ","").replace("-",",").split(','))!=3:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,2))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,2).replace(" ","").replace("-",",")+"]"
            
            straight=worksheet.cell_value(r,3)
            box=worksheet.cell_value(r,4)
            staright_and_box=worksheet.cell_value(r,5)
            box_only=worksheet.cell_value(r,6)
            
            
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.straight=straight
            _draw.box=box
            _draw.staright_and_box=staright_and_box
            _draw.box_only=box_only
            
#             print component
#             print result
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_DAILY4":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="CAB",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=4:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            straight=worksheet.cell_value(r,2)
            box=worksheet.cell_value(r,3)
            staright_and_box=worksheet.cell_value(r,4)
            box_only=worksheet.cell_value(r,5)
            
            
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.straight=straight
            _draw.box=box
            _draw.staright_and_box=staright_and_box
            _draw.box_only=box_only
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_FANTASY5":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="CA2",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=5:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            jackpot=worksheet.cell_value(r,2)
            four_of_five_only=worksheet.cell_value(r,3)
            three_of_five_only=worksheet.cell_value(r,4)
            two_of_five_only=worksheet.cell_value(r,5)
            
            
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.jackpot=jackpot
            _draw.four_of_five_only=four_of_five_only
            _draw.three_of_five_only=three_of_five_only
            _draw.two_of_five_only=two_of_five_only
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_SUPER_LOTTO_PLUS":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="CA1",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            jackpot= worksheet.cell_value(r,2)
            five_of_five_only=worksheet.cell_value(r,3)
            four_of_five_megaball=worksheet.cell_value(r,4)
            four_of_five_only=worksheet.cell_value(r,5)
            three_of_five_with_megaball=worksheet.cell_value(r,6)
            three_of_five_only=worksheet.cell_value(r,7)
            two_of_five_megaball=worksheet.cell_value(r,8)
            one_of_five_megaball=worksheet.cell_value(r,9)
            megaball=worksheet.cell_value(r,10)
            
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.jackpot= jackpot
            _draw.five_of_five_only=five_of_five_only
            _draw.four_of_five_megaball=four_of_five_megaball
            _draw.four_of_five_only=four_of_five_only
            _draw.three_of_five_with_megaball=three_of_five_with_megaball
            _draw.three_of_five_only=three_of_five_only
            _draw.two_of_five_megaball=two_of_five_megaball
            _draw.one_of_five_megaball=one_of_five_megaball
            _draw.megaball=megaball
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_POWERBALL":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="101",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            #powerplay = 
            jackpot =worksheet.cell_value(r,2)
            five_of_five_only = worksheet.cell_value(r,3)
            four_of_five_powerball = worksheet.cell_value(r,4)
            four_of_five_only = worksheet.cell_value(r,5)
            three_of_five_with_powerball = worksheet.cell_value(r,6)
            three_of_five_only = worksheet.cell_value(r,7)
            two_of_five_powerball = worksheet.cell_value(r,8)
            one_of_five_powerball =worksheet.cell_value(r,9)
            powerball_only =worksheet.cell_value(r,10)
            
            #two_of_five_only = 
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.jackpot = jackpot
            _draw.five_of_five_only = five_of_five_only
            _draw.four_of_five_powerball = four_of_five_powerball
            _draw.four_of_five_only = four_of_five_only
            _draw.three_of_five_with_powerball = three_of_five_with_powerball
            _draw.three_of_five_only = three_of_five_only
            _draw.two_of_five_powerball = two_of_five_powerball
            _draw.one_of_five_powerball =one_of_five_powerball
            _draw.powerball_only =powerball_only
            _draw.result=result
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="CA_MEGAMILLION":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="CA")
            component=LotteryGameComponent.objects.get(remote_id="113",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            jackpot=worksheet.cell_value(r,2)
            five_of_five_only=worksheet.cell_value(r,3)  
            four_of_five_megaball =worksheet.cell_value(r,4)
            four_of_five_only=worksheet.cell_value(r,5)
            three_of_five_with_megaball =worksheet.cell_value(r,6)
            three_of_five_only=worksheet.cell_value(r,7)
            two_of_five_megaball =worksheet.cell_value(r,8)
            one_of_five_megaball =worksheet.cell_value(r,9)
            megaball =worksheet.cell_value(r,10)
            
            #megaplier = 
             
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.jackpot=jackpot
            _draw.five_of_five_only=five_of_five_only
            _draw.four_of_five_megaball =four_of_five_megaball
            _draw.four_of_five_only=four_of_five_only
            _draw.three_of_five_with_megaball =three_of_five_with_megaball
            _draw.three_of_five_only=three_of_five_only
            _draw.two_of_five_megaball =two_of_five_megaball
            _draw.one_of_five_megaball =one_of_five_megaball
            _draw.megaball =megaball
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
            
        elif game=="TX_TEXAS_TWO_STEP":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            component=LotteryGameComponent.objects.get(remote_id="TX3",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=5:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            
            
            four_of_four_bonus = worksheet.cell_value(r,2)
            four_of_four =worksheet.cell_value(r,3)# doubt >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
            three_of_four_bonus =worksheet.cell_value(r,4)
            three_of_four =worksheet.cell_value(r,5)
            two_of_four_bonus = worksheet.cell_value(r,6)
            #two_of_four =worksheet.cell_value(r,2)
            one_of_four_bonus =worksheet.cell_value(r,7)
            bonus = worksheet.cell_value(r,8)
            #one_of_four = worksheet.cell_value(r,2)
            
            
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.four_of_four_bonus =four_of_four_bonus
            _draw.four_of_four =four_of_four
            _draw.three_of_four_bonus =three_of_four_bonus
            _draw.three_of_four =three_of_four
            _draw.two_of_four_bonus =two_of_four_bonus
            #two_of_four =worksheet.cell_value(r,2)
            _draw.one_of_four_bonus =one_of_four_bonus
            _draw.bonus = bonus
            #one_of_four = worksheet.cell_value(r,2)
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="TX_LOTTO_TEXAS":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            component=LotteryGameComponent.objects.get(remote_id="TX1",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=6:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            six_of_six_only =worksheet.cell_value(r,2)
            five_of_six_only = worksheet.cell_value(r,3)
            four_of_six_only = worksheet.cell_value(r,4)
            three_of_six_only =worksheet.cell_value(r,5)
            two_of_six_only = worksheet.cell_value(r,6)
            five_of_six_extra= worksheet.cell_value(r,7)
            four_of_six_extra= worksheet.cell_value(r,8)
            three_of_six_extra = worksheet.cell_value(r,9)
            two_of_six_extra= worksheet.cell_value(r,10)
                       
            
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.six_of_six_only =six_of_six_only
            _draw.five_of_six_only=five_of_six_only
            _draw.four_of_six_only=four_of_six_only
            _draw.three_of_six_only=three_of_six_only
            _draw.two_of_six_only=two_of_six_only
            
            _draw.five_of_six_extra=five_of_six_extra
            _draw.four_of_six_extra=four_of_six_extra
            _draw.three_of_six_extra=three_of_six_extra
            _draw.two_of_six_extra=two_of_six_extra
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        elif game=="TX_CASH_OF_FIVE":
            
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            component=LotteryGameComponent.objects.get(remote_id="TX2",division=division)
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            try:
                if len(worksheet.cell_value(r,1).replace(" ","").replace("-",",").split(','))!=5:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,1))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,1).replace(" ","").replace("-",",")+"]"
            
            five_of_five_only=worksheet.cell_value(r,2)
            four_of_five_only=worksheet.cell_value(r,3)
            three_of_five_only=worksheet.cell_value(r,4)
            two_of_five_only=worksheet.cell_value(r,5)
            
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.five_of_five_only=five_of_five_only
            _draw.four_of_five_only=four_of_five_only
            _draw.three_of_five_only=three_of_five_only
            _draw.two_of_five_only=two_of_five_only
            
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
        
        elif game=="TX_ALL_OR_NOTHING":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            division=LotteryCountryDivision.objects.get(remote_id="TX")
            date=datetime.datetime(tpl[0],tpl[1],tpl[2]).date()
            
            possible_components={
                                 'night':LotteryGameComponent.objects.get(remote_id="TXI",division=division),
                                 'evening':LotteryGameComponent.objects.get(remote_id="TXH",division=division),
                                 'day':LotteryGameComponent.objects.get(remote_id="TXG",division=division),
                                 'morning':LotteryGameComponent.objects.get(remote_id="TXF",division=division)
                                 }
            
            component=possible_components[worksheet.cell_value(r,1).lower().strip()]
            
            try:
                if len(worksheet.cell_value(r,2).replace(" ","").replace("-",",").split(','))!=12:
                    raise invalidExcelField("Invalid winning numbers data="+str(worksheet.cell_value(r,2))+" in excel row="+str(r+1))
            except invalidExcelField,e:
                print e.error_msg()
                raise RuntimeError("can't process !")
            
            result="["+worksheet.cell_value(r,2).replace(" ","").replace("-",",")+"]"
            
            twelve_of_twelve = worksheet.cell_value(r,3)
            eleven_of_twelve =worksheet.cell_value(r,4)
            ten_of_tweleve = worksheet.cell_value(r,5)
            nine_of_twelve = worksheet.cell_value(r,6)
            eight_of_twelve = worksheet.cell_value(r,7)
            four_of_twelve = worksheet.cell_value(r,8)
            three_of_twelve = worksheet.cell_value(r,9)
            two_of_twelve = worksheet.cell_value(r,10)
            one_of_twelve = worksheet.cell_value(r,11)
            zero_of_twelve = worksheet.cell_value(r,12)
            
            
            removeDuplicate(component=component,date=date,division=division)
            _draw,is_new=LotteryDraw.objects.get_or_create(component=component,date=date,division=division)
            _draw.result=result
            
            _draw.twelve_of_twelve = twelve_of_twelve
            _draw.eleven_of_twelve =eleven_of_twelve
            _draw.ten_of_tweleve = ten_of_tweleve
            _draw.nine_of_twelve = nine_of_twelve
            _draw.eight_of_twelve = eight_of_twelve
            _draw.four_of_twelve = four_of_twelve
            _draw.three_of_twelve =three_of_twelve
            _draw.two_of_twelve = two_of_twelve
            _draw.one_of_twelve = one_of_twelve
            _draw.zero_of_twelve = zero_of_twelve
            
            #print _draw.result,_draw.twelve_of_twelve,_draw.eleven_of_twelve,_draw.ten_of_tweleve,_draw.nine_of_twelve,eight_of_twelve
            _draw.save()
            print "row : "+str(r+1)+" saved,has Id :"+str(_draw.id)+" IS_NEW:"+str(is_new)
    
    print " << DONE >>"


'''
1:"TX_CASH_OF_FIVE"     sheet :0
2:"TX_ALL_OR_NOTHING"   sheet :1
3:"TX_LOTTO_TEXAS"      sheet :2
4:"TX_TEXAS_TWO_STEP"   sheet :3
12:"TX_POWERBALL"       sheet :4
13:"TX_MEGAMILLION"     sheet :5


5:"CA_MEGAMILLION"      sheet :0  
6:"CA_POWERBALL"        sheet :1
7:"CA_SUPER_LOTTO_PLUS" sheet :2
8:"CA_FANTASY5",        sheet :3
9:"CA_DAILY4",          sheet :4
10:"CA_DAILY3",         sheet :5
11:"CA_DAILY_DERBY"     sheet :6

 
'''
#take_dump('/home/irfan/Downloads/yoolotto_support/TX_DATA_NEW.xlsx',shortcut=1,sheet=0)


def take_dump_TX(excel_path):
    _workbook=[
                     (1,"TX_CASH_OF_FIVE",0),
                     (2,"TX_ALL_OR_NOTHING",1),
                     (3,"TX_LOTTO_TEXAS",2),
                     (4,"TX_TEXAS_TWO_STEP",3),
                     (12,"TX_POWERBALL",4),
                     (13,"TX_MEGAMILLION",5)
                     ]
    for shortcut,name,sheet in _workbook:
        #print shortcut,name,sheetmsg="Invalid Numbers "
        take_dump(excel_path,shortcut=shortcut,sheet=sheet)
    print "ALL FINISHED !"
    

def take_dump_CA(excel_path):
    _workbook=[
                     (5,"CA_MEGAMILLION",0),
                     (6,"CA_POWERBALL",1),
                     (7,"CA_SUPER_LOTTO_PLUS",2),
                     (8,"CA_FANTASY5",3),
                     (9,"CA_DAILY4",4),
                     (10,"CA_DAILY3",5),
                     (11,"CA_DAILY_DERBY",6)
                     ]
    for shortcut,name,sheet in _workbook:
        #print shortcut,name,sheet
        take_dump(excel_path,shortcut=shortcut,sheet=sheet)
    print "ALL FINISHED !"
    
    

#take_dump('/home/irfan/Downloads/yoolotto_support/TX_DATA_NEW.xlsx',shortcut=4,sheet=3)
    
take_dump_TX('/home/ubuntu/yo/yoolotto/yoolotto/lottery/feed/TX_DATA_NEW.xlsx')
take_dump_CA('/home/ubuntu/yo/yoolotto/yoolotto/lottery/feed/CA_DATA_NEW.xlsx')




















# 
# 
# import os
#   
# f = open(os.path.dirname(os.path.realpath(__file__))+"/import_excel.log",'w')
# f.write(str("TX_CASH5")+": excel row="+str(1)+" : draw id ="+str(1)+" : IS_NEW : "+str(True)+"\n")
# #f.write(str("TX_CASH5")+": excel row="+str(1)+" : draw id ="+str(1)+" : IS_NEW : "+str(True)+"\n")
# # python will convert \n to os.linesep
# f.close()
# 






