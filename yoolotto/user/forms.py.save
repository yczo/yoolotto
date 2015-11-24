from django import forms

class UserUpdateForm(forms.Form):
    email = forms.EmailField(required=False, error_messages={
        "required": "Please provide an Email Address",
        "invalid": "Please provide a valid Email Address"
    })
    
    password = forms.CharField(required=False, error_messages={
        "required": "Please provide an Password",
        "invalid": "Please choose a password with at least six characters"
    })
    
    # Legacy Names
    fbToken = forms.CharField(required=False)
    twitterToken = forms.CharField(required=False)
    pushToken = forms.CharField(required=False)
    
    referral = forms.CharField(required=False)
    
    # ???
    contestEmail = forms.CharField(required=False)
    requestedEmail = forms.CharField(required=False)
    
    def clean_password(self):
        data = self.cleaned_data.get("password", None)
        if not data:
            return
        
        if len(data) < 0:
            raise forms.ValidationError(self.fields["password"].error_messages["invalid"])
        
        return data
    
class PreferenceUpdateForm(forms.Form):
    jackpot_drawing = forms.BooleanField(required=False)
    jackpot_frenzy = forms.BooleanField(required=False)
    newsletter = forms.BooleanField(required=False)
