# -*- coding: utf-8 -*-
from south.utils import datetime_utils as datetime
from south.db import db
from south.v2 import SchemaMigration
from django.db import models


class Migration(SchemaMigration):

    def forwards(self, orm):
        # Adding model 'CouponVendor'
        db.create_table(u'coupon_vendor', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('image', self.gf('django.db.models.fields.files.ImageField')(max_length=100, null=True, blank=True)),
            ('wallet_url', self.gf('django.db.models.fields.CharField')(max_length=1024, null=True, blank=True)),
            ('coupon_url', self.gf('django.db.models.fields.CharField')(max_length=1024, null=True, blank=True)),
            ('coupon_mini_url', self.gf('django.db.models.fields.CharField')(max_length=1024, null=True, blank=True)),
            ('referral', self.gf('django.db.models.fields.CharField')(max_length=128, null=True, blank=True)),
            ('email_content', self.gf('django.db.models.fields.files.FileField')(max_length=100, null=True, blank=True)),
        ))
        db.send_create_signal(u'coupon', ['CouponVendor'])

        # Adding model 'CouponVendorPromo'
        db.create_table(u'coupon_vendor_promo', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('promo', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('vendor', self.gf('django.db.models.fields.related.ForeignKey')(related_name='promos', to=orm['coupon.CouponVendor'])),
            ('valid_from', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
            ('valid_to', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'coupon', ['CouponVendorPromo'])

        # Adding model 'Coupon'
        db.create_table(u'coupon', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('title', self.gf('django.db.models.fields.CharField')(max_length=255)),
            ('description', self.gf('django.db.models.fields.CharField')(max_length=255, null=True, blank=True)),
            ('vendor', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='coupons', null=True, to=orm['coupon.CouponVendor'])),
            ('url', self.gf('django.db.models.fields.URLField')(max_length=200, null=True, blank=True)),
            ('image', self.gf('django.db.models.fields.files.ImageField')(max_length=100, null=True, blank=True)),
            ('geography', self.gf('django.db.models.fields.related.ForeignKey')(to=orm['coupon.CouponGeography'], null=True, blank=True)),
            ('allocate_tickets', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('allocate_wallet', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('valid_from', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
            ('valid_to', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
            ('redeem_limit', self.gf('django.db.models.fields.IntegerField')(null=True, blank=True)),
            ('redeem_strict', self.gf('django.db.models.fields.BooleanField')(default=False)),
            ('redeemable', self.gf('django.db.models.fields.BooleanField')(default=True)),
            ('issued_total', self.gf('django.db.models.fields.IntegerField')(default=0)),
            ('issued_redeemed', self.gf('django.db.models.fields.IntegerField')(default=0)),
            ('sequence', self.gf('django.db.models.fields.IntegerField')(default=0)),
            ('vote_liked', self.gf('django.db.models.fields.IntegerField')(default=0)),
            ('vote_disliked', self.gf('django.db.models.fields.IntegerField')(default=0)),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
        ))
        db.send_create_signal(u'coupon', ['Coupon'])

        # Adding model 'CouponGeography'
        db.create_table(u'coupon_geography', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('name', self.gf('django.db.models.fields.CharField')(unique=True, max_length=255)),
            ('active', self.gf('django.db.models.fields.BooleanField')(default=True)),
            ('format', self.gf('django.db.models.fields.CharField')(max_length=255)),
        ))
        db.send_create_signal(u'coupon', ['CouponGeography'])

        # Adding model 'CouponVote'
        db.create_table(u'coupon_vote', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('coupon', self.gf('django.db.models.fields.related.ForeignKey')(related_name='votes', to=orm['coupon.Coupon'])),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(related_name='coupon_votes', to=orm['user.YooLottoUser'])),
            ('liked', self.gf('django.db.models.fields.BooleanField')()),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
            ('updated_at', self.gf('django.db.models.fields.DateTimeField')(auto_now=True, blank=True)),
        ))
        db.send_create_signal(u'coupon', ['CouponVote'])

        # Adding unique constraint on 'CouponVote', fields ['coupon', 'user']
        db.create_unique(u'coupon_vote', ['coupon_id', 'user_id'])

        # Adding model 'CouponShare'
        db.create_table(u'coupon_share', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('coupon', self.gf('django.db.models.fields.related.ForeignKey')(related_name='shares', to=orm['coupon.Coupon'])),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(related_name='coupon_shares', to=orm['user.YooLottoUser'])),
            ('type', self.gf('django.db.models.fields.CharField')(max_length=255)),
        ))
        db.send_create_signal(u'coupon', ['CouponShare'])

        # Adding model 'CouponIssue'
        db.create_table(u'coupon_issue', (
            (u'id', self.gf('django.db.models.fields.AutoField')(primary_key=True)),
            ('coupon', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='issued', null=True, to=orm['coupon.Coupon'])),
            ('promo', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='issued', null=True, to=orm['coupon.CouponVendorPromo'])),
            ('ticket', self.gf('django.db.models.fields.related.ForeignKey')(blank=True, related_name='coupon_issue', unique=True, null=True, to=orm['lottery.LotteryTicket'])),
            ('user', self.gf('django.db.models.fields.related.ForeignKey')(related_name='coupon_issue', to=orm['user.YooLottoUser'])),
            ('added_at', self.gf('django.db.models.fields.DateTimeField')(auto_now_add=True, blank=True)),
            ('redeemed', self.gf('django.db.models.fields.DateTimeField')(null=True, blank=True)),
        ))
        db.send_create_signal(u'coupon', ['CouponIssue'])


    def backwards(self, orm):
        # Removing unique constraint on 'CouponVote', fields ['coupon', 'user']
        db.delete_unique(u'coupon_vote', ['coupon_id', 'user_id'])

        # Deleting model 'CouponVendor'
        db.delete_table(u'coupon_vendor')

        # Deleting model 'CouponVendorPromo'
        db.delete_table(u'coupon_vendor_promo')

        # Deleting model 'Coupon'
        db.delete_table(u'coupon')

        # Deleting model 'CouponGeography'
        db.delete_table(u'coupon_geography')

        # Deleting model 'CouponVote'
        db.delete_table(u'coupon_vote')

        # Deleting model 'CouponShare'
        db.delete_table(u'coupon_share')

        # Deleting model 'CouponIssue'
        db.delete_table(u'coupon_issue')


    models = {
        u'coupon.coupon': {
            'Meta': {'object_name': 'Coupon', 'db_table': "u'coupon'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'allocate_tickets': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'allocate_wallet': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'description': ('django.db.models.fields.CharField', [], {'max_length': '255', 'null': 'True', 'blank': 'True'}),
            'geography': ('django.db.models.fields.related.ForeignKey', [], {'to': u"orm['coupon.CouponGeography']", 'null': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'image': ('django.db.models.fields.files.ImageField', [], {'max_length': '100', 'null': 'True', 'blank': 'True'}),
            'issued_redeemed': ('django.db.models.fields.IntegerField', [], {'default': '0'}),
            'issued_total': ('django.db.models.fields.IntegerField', [], {'default': '0'}),
            'redeem_limit': ('django.db.models.fields.IntegerField', [], {'null': 'True', 'blank': 'True'}),
            'redeem_strict': ('django.db.models.fields.BooleanField', [], {'default': 'False'}),
            'redeemable': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'sequence': ('django.db.models.fields.IntegerField', [], {'default': '0'}),
            'title': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'url': ('django.db.models.fields.URLField', [], {'max_length': '200', 'null': 'True', 'blank': 'True'}),
            'valid_from': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'valid_to': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'vendor': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'coupons'", 'null': 'True', 'to': u"orm['coupon.CouponVendor']"}),
            'vote_disliked': ('django.db.models.fields.IntegerField', [], {'default': '0'}),
            'vote_liked': ('django.db.models.fields.IntegerField', [], {'default': '0'})
        },
        u'coupon.coupongeography': {
            'Meta': {'object_name': 'CouponGeography', 'db_table': "u'coupon_geography'"},
            'active': ('django.db.models.fields.BooleanField', [], {'default': 'True'}),
            'format': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'unique': 'True', 'max_length': '255'})
        },
        u'coupon.couponissue': {
            'Meta': {'object_name': 'CouponIssue', 'db_table': "u'coupon_issue'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'coupon': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'issued'", 'null': 'True', 'to': u"orm['coupon.Coupon']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'promo': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'issued'", 'null': 'True', 'to': u"orm['coupon.CouponVendorPromo']"}),
            'redeemed': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'ticket': ('django.db.models.fields.related.ForeignKey', [], {'blank': 'True', 'related_name': "'coupon_issue'", 'unique': 'True', 'null': 'True', 'to': u"orm['lottery.LotteryTicket']"}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'coupon_issue'", 'to': u"orm['user.YooLottoUser']"})
        },
        u'coupon.couponshare': {
            'Meta': {'object_name': 'CouponShare', 'db_table': "u'coupon_share'"},
            'coupon': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'shares'", 'to': u"orm['coupon.Coupon']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'type': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'coupon_shares'", 'to': u"orm['user.YooLottoUser']"})
        },
        u'coupon.couponvendor': {
            'Meta': {'object_name': 'CouponVendor', 'db_table': "u'coupon_vendor'"},
            'coupon_mini_url': ('django.db.models.fields.CharField', [], {'max_length': '1024', 'null': 'True', 'blank': 'True'}),
            'coupon_url': ('django.db.models.fields.CharField', [], {'max_length': '1024', 'null': 'True', 'blank': 'True'}),
            'email_content': ('django.db.models.fields.files.FileField', [], {'max_length': '100', 'null': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'image': ('django.db.models.fields.files.ImageField', [], {'max_length': '100', 'null': 'True', 'blank': 'True'}),
            'name': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'referral': ('django.db.models.fields.CharField', [], {'max_length': '128', 'null': 'True', 'blank': 'True'}),
            'wallet_url': ('django.db.models.fields.CharField', [], {'max_length': '1024', 'null': 'True', 'blank': 'True'})
        },
        u'coupon.couponvendorpromo': {
            'Meta': {'object_name': 'CouponVendorPromo', 'db_table': "u'coupon_vendor_promo'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'promo': ('django.db.models.fields.CharField', [], {'max_length': '255'}),
            'valid_from': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'valid_to': ('django.db.models.fields.DateTimeField', [], {'null': 'True', 'blank': 'True'}),
            'vendor': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'promos'", 'to': u"orm['coupon.CouponVendor']"})
        },
        u'coupon.couponvote': {
            'Meta': {'unique_together': "(('coupon', 'user'),)", 'object_name': 'CouponVote', 'db_table': "u'coupon_vote'"},
            'added_at': ('django.db.models.fields.DateTimeField', [], {'auto_now_add': 'True', 'blank': 'True'}),
            'coupon': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'votes'", 'to': u"orm['coupon.Coupon']"}),
            u'id': ('django.db.models.fields.AutoField', [], {'primary_key': 'True'}),
            'liked': ('django.db.models.fields.BooleanField', [], {}),
            'updated_at': ('django.db.models.fields.DateTimeField', [], {'auto_now': 'True', 'blank': 'True'}),
            'user': ('django.db.models.fields.related.ForeignKey', [], {'related_name': "'coupon_votes'", 'to': u"orm['user.YooLottoUser']"})
        },
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

    complete_apps = ['coupon']