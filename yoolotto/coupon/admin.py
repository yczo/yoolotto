from django.contrib import admin
from yoolotto.coupon.models import *


class CouponAdmin(admin.ModelAdmin):
    """
    displaying the coupon vendor page
    """
    list_display = ['id','description', 'url', '_image_tag']

    def _image_tag(self, obj):
        return """<img src="/%s" width="200" height="200" />""" % (obj.image)
    _image_tag.short_description = "Image Preview"
    _image_tag.allow_tags = True

    # def save_model(self, request, obj, form, change):
    #     if obj.url and obj.image:
    #         obj.url = os.path.join(settings.BASE_URL,obj.image._get_url() + "?identity=%s")
    #         obj.save()
        
admin.site.register(Coupon, CouponAdmin)

class CouponVendorAdmin(admin.ModelAdmin):
    """
    displaying the adin class
    """
    
    list_display = ['id','name', 'wallet_url', '_image_tag',]
    
    fieldsets = (
        ('Vendor Details', {
            'fields': ('name', 'image')
        }),
        ('Upload Custom Email Template', {
            'classes': ('collapse',),
            'fields': ('email_content',)
        }),  
        ('Coupon URLS', {
            'classes': ('collapse',),
            'fields': ('wallet_url',)
        }),
        ('Referral URLS', {
            'classes': ('collapse',),
            'fields': ('referral',)
        }),
        ('Coupon URLS', {
            'classes': ('collapse',),
            'fields': ('coupon_url',)
        }),
    )
    
    def _image_tag(self, obj):
        return """<img src="/%s" width="100" height="100" />""" % (obj.image)
    _image_tag.short_description = 'Image Preview'
    _image_tag.allow_tags = True
    

admin.site.register(CouponVendor, CouponVendorAdmin)
