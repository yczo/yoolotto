# -*- coding: utf-8 -*-
from south.utils import datetime_utils as datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Adding model 'LotteryCountryDivision'
        db.create_table(u'lottery_country_division', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('remote_id', self.gf('django.db.models.fields.CharField')(unique=True, max_length=8)),
            ('remote_country', self.gf('django.db.models.fields.CharField')(max_length=32)),
        ))
        db.send_create_signal(u'lottery', ['LotteryCountryDivision'])

        # Adding model 'LotteryGame'
        db.create_table(u'lottery_game', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('code', self.gf('django.db.models.fields.CharField')(unique=True, max_length=64)),
            ('active', self.gf('django.db.models.fields.BooleanField')(default=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryGame'])

        # Adding model 'LotteryGameComponent'
        db.create_table(u'lottery_game_component', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('parent', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='components', null=True, to=orm['lottery.LotteryGame'])),
            ('remote_id', self.gf('django.db.models.fields.CharField')(max_length=8)),
            ('active', self.gf('django.db.models.fields.BooleanField')(default=True)),
            ('format', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('identifier', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryGameComponent'])

        # Adding M2M table for field division on 'LotteryGameComponent'
        m2m_table_name = db.shorten_name(u'lottery_game_component_division')
        db.create_table(m2m_table_name, (
            ('id', models.AutoField(verbose_name='ID', primary_key=True, auto_created=True)),
            ('lotterygamecomponent', models.ForeignKey(orm[u'lottery.lotterygamecomponent'], null=False)),
            ('lotterycountrydivision', models.ForeignKey(orm[u'lottery.lotterycountrydivision'], null=False))
        ))
        db.create_unique(m2m_table_name, ['lotterygamecomponent_id', 'lotterycountrydivision_id'])

        # Adding model 'LotteryDraw'
        db.create_table(u'lottery_draw', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('component', self.gf('django.db.models.fields.related.ForeignKey')(related_name='draws', to=orm['lottery.LotteryGameComponent'])),
            ('date', self.gf('django.db.models.fields.DateField')()),
            ('jackpot', self.gf('django.db.models.fields.IntegerField')(null=True, blank=True)),
            ('result', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('division', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['lottery.LotteryCountryDivision'], null=True)),
            ('official', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('frenzied', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('powerplay', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('five_of_five_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('five_of_five_powerplay', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_five_powerball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_five_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_five_with_powerball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_five_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_five_powerball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_five_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('one_of_five_powerball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('powerball_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('megaplier', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('megaball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_five_megaball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_five_with_megaball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_five_megaball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('one_of_five_megaball', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('extra', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('six_of_six_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('five_of_six_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('five_of_six_extra', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_six_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_six_extra', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_six_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_six_extra', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_six_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_six_extra', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('bonus', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_four', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_four_bonus', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_four', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_four_bonus', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_four', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_four_bonus', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('one_of_four', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('one_of_four_bonus', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('straight', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('box', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('staright_and_box', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('box_only', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('win', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('exacta', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('trifecta', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('race_time', self.gf('django.db.models.fields.CharField')(max_length=80, null=True, blank=True)),
            ('exacta_with_racetime', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('win_with_racetime', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('race_time_amount', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('twelve_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('eleven_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('ten_of_tweleve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('nine_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('eight_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('four_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('three_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('two_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('one_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('zero_of_twelve', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('updated', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
            ('number_of_winners', self.gf('django.db.models.fields.IntegerField')(default=0, null=True, blank=True)),
            ('odds', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryDraw'])

        # Adding model 'LotteryDrawFrenzy'
        db.create_table(u'lottery_draw_frenzy', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('draw', self.gf('django.db.models.fields.related.ForeignKey')(related_name='frenzies', to=orm['lottery.LotteryDraw'])),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryDrawFrenzy'])

        # Adding model 'LotteryTicket'
        db.create_table(u'lottery_ticket', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('draw', self.gf('django.db.models.fields.related.ForeignKey')(related_name='tickets', to=orm['lottery.LotteryDraw'])),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(related_name='tickets', to=orm['user.YooLottoUser'])),
            ('division', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['lottery.LotteryCountryDivision'], null=True)),
            ('winnings', self.gf('django.db.models.fields.DecimalField')(null=True, max_digits=15, decimal_places=2, blank=True)),
            ('notified', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('deleted', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicket'])

        # Adding model 'LotteryTicketClient'
        db.create_table(u'lottery_ticket_client', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('draw', self.gf('django.db.models.fields.related.ForeignKey')(related_name='tickets_client', to=orm['lottery.LotteryDraw'])),
            ('device', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('email', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicketClient'])

        # Adding model 'LotteryTicketSubmission'
        db.create_table(u'lottery_ticket_submission', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('submission', self.gf('django.db.models.fields.CharField')(max_length=16, null=True, blank=True)),
            ('ticket', self.gf('django.db.models.fields.related.ForeignKey')(related_name='submissions', to=orm['lottery.LotteryTicket'])),
            ('checked', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicketSubmission'])

        # Adding model 'LotteryTicketPlay'
        db.create_table(u'lottery_ticket_play', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('ticket', self.gf('django.db.models.fields.related.ForeignKey')(related_name='plays', to=orm['lottery.LotteryTicket'])),
            ('play', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('division', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['lottery.LotteryCountryDivision'], null=True)),
            ('submission_old', self.gf('django.db.models.fields.CharField')(max_length=16, null=True, db_column='submission', blank=True)),
            ('submission', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='plays', null=True, to=orm['lottery.LotteryTicketSubmission'])),
            ('winnings', self.gf('django.db.models.fields.DecimalField')(null=True, max_digits=15, decimal_places=2, blank=True)),
            ('winnings_base', self.gf('django.db.models.fields.DecimalField')(null=True, max_digits=15, decimal_places=2, blank=True)),
            ('winnings_sum', self.gf('django.db.models.fields.DecimalField')(null=True, max_digits=15, decimal_places=2, blank=True)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicketPlay'])

        # Adding model 'LotteryTicketAvailable'
        db.create_table(u'lottery_ticket_available', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('ticket', self.gf('django.db.models.fields.related.ForeignKey')(related_name='ticket_submissions', to=orm['lottery.LotteryTicket'])),
            ('play', self.gf('django.db.models.fields.related.ForeignKey')(related_name='ticket_play', to=orm['lottery.LotteryTicketPlay'])),
            ('available', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('json', self.gf('django.db.models.fields.TextField')()),
            ('device', self.gf('django.db.models.fields.CharField')(max_length=256)),
            ('image_first', self.gf('django.db.models.fields.files.ImageField')(max_length=300, null=True, blank=True)),
            ('image_second', self.gf('django.db.models.fields.files.ImageField')(max_length=300, null=True, blank=True)),
            ('image_third', self.gf('django.db.models.fields.files.ImageField')(max_length=300, null=True, blank=True)),
            ('valid_image_name', self.gf('django.db.models.fields.TextField')(max_length=256, null=True, blank=True)),
            ('rejected', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('reason', self.gf('django.db.models.fields.TextField')(null=True, blank=True)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicketAvailable'])

        # Adding model 'LotteryTicketEdit'
        db.create_table(u'lottery_ticket_edit', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('available', self.gf('django.db.models.fields.related.ForeignKey')(related_name='ticket_available', to=orm['lottery.LotteryTicketAvailable'])),
            ('numbers', self.gf('django.db.models.fields.CharField')(max_length=256)),
        ))
        db.send_create_signal(u'lottery', ['LotteryTicketEdit'])


    def backwards(self, orm):
        # Deleting model 'LotteryCountryDivision'
        db.delete_table(u'lottery_country_division')

        # Deleting model 'LotteryGame'
        db.delete_table(u'lottery_game')

        # Deleting model 'LotteryGameComponent'
        db.delete_table(u'lottery_game_component')

        # Removing M2M table for field division on 'LotteryGameComponent'
        db.delete_table(db.shorten_name(u'lottery_game_component_division'))

        # Deleting model 'LotteryDraw'
        db.delete_table(u'lottery_draw')

        # Deleting model 'LotteryDrawFrenzy'
        db.delete_table(u'lottery_draw_frenzy')

        # Deleting model 'LotteryTicket'
        db.delete_table(u'lottery_ticket')

        # Deleting model 'LotteryTicketClient'
        db.delete_table(u'lottery_ticket_client')

        # Deleting model 'LotteryTicketSubmission'
        db.delete_table(u'lottery_ticket_submission')

        # Deleting model 'LotteryTicketPlay'
        db.delete_table(u'lottery_ticket_play')

        # Deleting model 'LotteryTicketAvailable'
        db.delete_table(u'lottery_ticket_available')

        # Deleting model 'LotteryTicketEdit'
        db.delete_table(u'lottery_ticket_edit')


    models = {
        u'lottery.lotterycountrydivision': {
            'Meta': {'object_name': 'LotteryCountryDivision', 'db_table': "u'lottery_country_division'"},
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'remote_country': ('django.db.models.fields.CharField', [], {'max_length': '32'}),
            'remote_id': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '8'})
        },
        u'lottery.lotterydraw': {
            'Meta': {'object_name': 'LotteryDraw', 'db_table': "u'lottery_draw'"},
            'bonus': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'box': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'box_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'component': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'draws'", 'to': u"orm['lottery.LotteryGameComponent']"}),
            'date': ('django.db.models.fields.DateField', [], {}),
            'division': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['lottery.LotteryCountryDivision']", 'null': 'True'}),
            'eight_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'eleven_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'exacta': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'exacta_with_racetime': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'extra': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'five_of_five_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'five_of_five_powerplay': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'five_of_six_extra': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'five_of_six_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_five_megaball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_five_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_five_powerball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_four': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_four_bonus': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_six_extra': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_six_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'four_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'frenzied': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'jackpot': ('django.db.models.fields.IntegerField', [], {'null': 'True', 'blank': 'True'}),
            'megaball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'megaplier': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'nine_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'number_of_winners': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'odds': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'official': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'one_of_five_megaball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'one_of_five_powerball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'one_of_four': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'one_of_four_bonus': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'one_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'powerball_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'powerplay': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'race_time': ('django.db.models.fields.CharField', [], {'max_length': '80', 'null': 'True', 'blank': 'True'}),
            'race_time_amount': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'result': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'six_of_six_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'staright_and_box': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'straight': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'ten_of_tweleve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_five_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_five_with_megaball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_five_with_powerball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_four': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_four_bonus': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_six_extra': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_six_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'three_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'trifecta': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'twelve_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_five_megaball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_five_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_five_powerball': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_four': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_four_bonus': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_six_extra': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_six_only': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'two_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'updated': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'win': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'win_with_racetime': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'}),
            'zero_of_twelve': ('django.db.models.fields.IntegerField', [], {'default': '0', 'null': 'True', 'blank': 'True'})
        },
        u'lottery.lotterydrawfrenzy': {
            'Meta': {'object_name': 'LotteryDrawFrenzy', 'db_table': "u'lottery_draw_frenzy'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'draw': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'frenzies'", 'to': u"orm['lottery.LotteryDraw']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'})
        },
        u'lottery.lotterygame': {
            'Meta': {'object_name': 'LotteryGame', 'db_table': "u'lottery_game'"},
            'active': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'code': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '64'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '255'})
        },
        u'lottery.lotterygamecomponent': {
            'Meta': {'object_name': 'LotteryGameComponent', 'db_table': "u'lottery_game_component'"},
            'active': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'division': ('django.db.models.fields.related.ManyToManyField', [], {'related_name': "'components'", 'symmetrical': 'False', 'to': u"orm['lottery.LotteryCountryDivision']"}),
            'format': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'identifier': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'parent': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'components'", 'null': 'True', 'to': u"orm['lottery.LotteryGame']"}),
            'remote_id': ('django.db.models.fields.CharField', [], {'max_length': '8'})
        },
        u'lottery.lotteryticket': {
            'Meta': {'object_name': 'LotteryTicket', 'db_table': "u'lottery_ticket'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'deleted': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'division': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['lottery.LotteryCountryDivision']", 'null': 'True'}),
            'draw': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'tickets'", 'to': u"orm['lottery.LotteryDraw']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'notified': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'tickets'", 'to': u"orm['user.YooLottoUser']"}),
            'winnings': ('django.db.models.fields.DecimalField', [], {'null': 'True', 'max_digits': '15', 'decimal_places': '2', 'blank': 'True'})
        },
        u'lottery.lotteryticketavailable': {
            'Meta': {'object_name': 'LotteryTicketAvailable', 'db_table': "u'lottery_ticket_available'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'available': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'device': ('django.db.models.fields.CharField', [], {'max_length': '256'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'image_first': ('django.db.models.fields.files.ImageField', [], {'max_length': '300', 'null': 'True', 'blank': 'True'}),
            'image_second': ('django.db.models.fields.files.ImageField', [], {'max_length': '300', 'null': 'True', 'blank': 'True'}),
            'image_third': ('django.db.models.fields.files.ImageField', [], {'max_length': '300', 'null': 'True', 'blank': 'True'}),
            'json': ('django.db.models.fields.TextField', [], {}),
            'play': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'ticket_play'", 'to': u"orm['lottery.LotteryTicketPlay']"}),
            'reason': ('django.db.models.fields.TextField', [], {'null': 'True', 'blank': 'True'}),
            'rejected': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'ticket': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'ticket_submissions'", 'to': u"orm['lottery.LotteryTicket']"}),
            'valid_image_name': ('django.db.models.fields.TextField', [], {'max_length': '256', 'null': 'True', 'blank': 'True'})
        },
        u'lottery.lotteryticketclient': {
            'Meta': {'object_name': 'LotteryTicketClient', 'db_table': "u'lottery_ticket_client'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'device': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'draw': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'tickets_client'", 'to': u"orm['lottery.LotteryDraw']"}),
            'email': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'})
        },
        u'lottery.lotteryticketedit': {
            'Meta': {'object_name': 'LotteryTicketEdit', 'db_table': "u'lottery_ticket_edit'"},
            'available': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'ticket_available'", 'to': u"orm['lottery.LotteryTicketAvailable']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'numbers': ('django.db.models.fields.CharField', [], {'max_length': '256'})
        },
        u'lottery.lotteryticketplay': {
            'Meta': {'object_name': 'LotteryTicketPlay', 'db_table': "u'lottery_ticket_play'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'division': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['lottery.LotteryCountryDivision']", 'null': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'play': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'submission': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'plays'", 'null': 'True', 'to': u"orm['lottery.LotteryTicketSubmission']"}),
            'submission_old': ('django.db.models.fields.CharField', [], {'max_length': '16', 'null': 'True', 'db_column': "'submission'", 'blank': 'True'}),
            'ticket': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'plays'", 'to': u"orm['lottery.LotteryTicket']"}),
            'winnings': ('django.db.models.fields.DecimalField', [], {'null': 'True', 'max_digits': '15', 'decimal_places': '2', 'blank': 'True'}),
            'winnings_base': ('django.db.models.fields.DecimalField', [], {'null': 'True', 'max_digits': '15', 'decimal_places': '2', 'blank': 'True'}),
            'winnings_sum': ('django.db.models.fields.DecimalField', [], {'null': 'True', 'max_digits': '15', 'decimal_places': '2', 'blank': 'True'})
        },
        u'lottery.lotteryticketsubmission': {
            'Meta': {'object_name': 'LotteryTicketSubmission', 'db_table': "u'lottery_ticket_submission'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'checked': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'submission': ('django.db.models.fields.CharField', [], {'max_length': '16', 'null': 'True', 'blank': 'True'}),
            'ticket': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'submissions'", 'to': u"orm['lottery.LotteryTicket']"})
        },
        u'user.yoolottouser': {
            'Meta': {'object_name': 'YooLottoUser', 'db_table': "u'user'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'email': ('django.db.models.fields.CharField', [], {'max_length': '255', 'unique': 'True', 'null': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'identifier': ('django.db.models.fields.CharField', [], {'max_length': '32', 'unique': 'True', 'null': 'True', 'blank': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'password': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'referral': ('django.db.models.fields.CharField', [], {'max_length': '64', 'null': 'True', 'blank': 'True'})
        }
    }

    complete_apps = ['lottery']