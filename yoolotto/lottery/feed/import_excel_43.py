from yoolotto.lottery.models import LotteryDraw, LotteryGameComponent,LotteryCountryDivision

from django.db import transaction

class invalidExcelField(Exception):
    def __init__(self,msg=None,value=None,row=None):
        self.value=value
        self.msg=msg
        self.row=row
    
    def error_msg(self):
        return self.msg if self.msg else "Invalid data="+str(self.value)+" for excel row="+str(self.row)+" !"
    


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
            
            
@transaction.commit_on_success
def take_dump(path,shortcut,sheet,stat_remote_id):
    import xlrd 
    import datetime
    workbook = xlrd.open_workbook(path)
    worksheet = workbook.sheet_by_index(sheet)
    
    xlrd_type={0:'empty',1:'text',2:'number',3:'date',4:'bool',5:'error code',6:''}
    game_shortcuts={
                          
                          12:"TX_POWERBALL",
                          13:"TX_MEGAMILLION"
                          }
        
        
        
    game=game_shortcuts[shortcut]#import for game
    print "FILLING FOR : "+game_shortcuts[shortcut]
    print "-"*50
    
    for r in range(1,worksheet.nrows):
        if game=="TX_MEGAMILLION":
            tpl=xlrd.xldate_as_tuple(worksheet.cell_value(r,0),workbook.datemode)
            
            division=LotteryCountryDivision.objects.get(remote_id=stat_remote_id)
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
            
            division=LotteryCountryDivision.objects.get(remote_id=stat_remote_id)
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
    print " << DONE >>"


def take_manual_dump(file_path,state_ids):
    _workbook=[
                     (12,"TX_POWERBALL",4),
                     (13,"TX_MEGAMILLION",5)
                     ]
    for s_id in state_ids:
        print "PICKED UP = "+s_id
        for shortcut,name,sheet in _workbook:
            #print shortcut,name,sheetmsg="Invalid Numbers "
            take_dump(file_path,shortcut=shortcut,sheet=sheet,stat_remote_id=s_id)
    print "ALL FINISHED !"
    





take_manual_dump(
                 file_path="/home/ubuntu/current/yobackend/yoolotto/yoolotto/lottery/feed/TX_DATA_NEW.xlsx",
                 state_ids=["NY","VT","NH","ME","MA","RI","CT","NJ","MD","DE","DC","PA","VA","NC","OH","WV","SC","GA","FL","MI","IN","KY","TN","WI","IL","MN","IA","MO","AR","LA","ND","SD","NE","KS","OK","CO","NM","AZ","ID","WA","OR","MT"]
                 )
    

















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






