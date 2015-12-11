from django import forms

class CouponGeographyLookupForm(forms.Form):
    latitude = forms.FloatField(required=True)
    longitude = forms.FloatField(required=True)