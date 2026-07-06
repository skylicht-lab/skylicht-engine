#ifdef IOS

#import "InputOverlayViewController.h"
#include <string>

std::string g_defaultText;
int g_requestHeight = 50;
int g_maxLength = 64;
bool g_secureInput = false;

void textfield_show(const char* text, int maxLength, int height, bool password)
{
	g_defaultText = text != NULL ? text : "";
	g_requestHeight = height;
	g_maxLength = maxLength;
	g_secureInput = password;
	
	InputOverlayViewController *inputVC = [[InputOverlayViewController alloc] init];
	inputVC.modalPresentationStyle = UIModalPresentationOverFullScreen;
	
	[[[[UIApplication sharedApplication] keyWindow] rootViewController] presentViewController:inputVC animated:YES completion:nil];
}

void textfield_on_change(const char* text);
void textfield_on_done(const char* text);

@interface InputOverlayViewController () <UITextFieldDelegate>

@property (nonatomic, strong) UITextField *inputTextField;
@property (nonatomic, strong) UIView *inputContainerView;
@property (nonatomic, strong) NSLayoutConstraint *inputContainerBottomConstraint;
@property (nonatomic, strong) UIButton *togglePasswordButton;

@end

@implementation InputOverlayViewController

- (void)viewDidLoad {
	[super viewDidLoad];
	self.view.backgroundColor = [UIColor clearColor];
	
	[self setupCustomInputView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillShow:)
												 name:UIKeyboardWillShowNotification
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillHide:)
												 name:UIKeyboardWillHideNotification
											   object:nil];
}

- (void)setupCustomInputView {
	NSLayoutXAxisAnchor *leadingAnchor = self.view.leadingAnchor;
	NSLayoutXAxisAnchor *trailingAnchor = self.view.trailingAnchor;
	if (@available(iOS 11.0, *)) {
		leadingAnchor = self.view.safeAreaLayoutGuide.leadingAnchor;
		trailingAnchor = self.view.safeAreaLayoutGuide.trailingAnchor;
	}
	
	// 1. Container
	self.inputContainerView = [[UIView alloc] init];
	self.inputContainerView.backgroundColor = [UIColor colorWithWhite:0.95 alpha:1.0];
	self.inputContainerView.translatesAutoresizingMaskIntoConstraints = NO;
	[self.view addSubview:self.inputContainerView];
	
	// 2. UITextField
	self.inputTextField = [[UITextField alloc] init];
	self.inputTextField.translatesAutoresizingMaskIntoConstraints = NO;
	self.inputTextField.borderStyle = UITextBorderStyleRoundedRect;
	self.inputTextField.font = [UIFont systemFontOfSize:16];
	self.inputTextField.autocorrectionType = UITextAutocorrectionTypeNo;
	self.inputTextField.autocapitalizationType = UITextAutocapitalizationTypeNone;
	self.inputTextField.returnKeyType = UIReturnKeyDone;
	self.inputTextField.enablesReturnKeyAutomatically = NO;
	self.inputTextField.delegate = self;
	self.inputTextField.text = [NSString stringWithUTF8String:g_defaultText.c_str()];
	self.inputTextField.secureTextEntry = g_secureInput;
	[self.inputTextField addTarget:self action:@selector(textFieldDidChange:) forControlEvents:UIControlEventEditingChanged];
	[self.inputContainerView addSubview:self.inputTextField];
	
	if (g_secureInput) {
		[self configurePasswordToggleButton];
	} else {
		self.inputTextField.clearButtonMode = UITextFieldViewModeWhileEditing;
	}
	
	// 3. UIButton
	UIButton *sendButton = [UIButton buttonWithType:UIButtonTypeSystem];
	[sendButton setTitle:@"Done" forState:UIControlStateNormal];
	sendButton.translatesAutoresizingMaskIntoConstraints = NO;
	[sendButton addTarget:self action:@selector(sendInputTapped) forControlEvents:UIControlEventTouchUpInside];
	[self.inputContainerView addSubview:sendButton];
	
	// 4.  Auto Layout Constraints
	self.inputContainerBottomConstraint = [self.inputContainerView.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:0];
	
	[NSLayoutConstraint activateConstraints:@[
		[self.inputContainerView.leadingAnchor constraintEqualToAnchor:leadingAnchor],
		[self.inputContainerView.trailingAnchor constraintEqualToAnchor:trailingAnchor],
		[self.inputContainerView.heightAnchor constraintEqualToConstant:MAX(g_requestHeight, 50)],
		self.inputContainerBottomConstraint,
		
		[self.inputTextField.topAnchor constraintEqualToAnchor:self.inputContainerView.topAnchor constant:8],
		[self.inputTextField.bottomAnchor constraintEqualToAnchor:self.inputContainerView.bottomAnchor constant:-8],
		[self.inputTextField.leadingAnchor constraintEqualToAnchor:self.inputContainerView.leadingAnchor constant:10],
		
		[sendButton.trailingAnchor constraintEqualToAnchor:self.inputContainerView.trailingAnchor constant:-10],
		[sendButton.centerYAnchor constraintEqualToAnchor:self.inputTextField.centerYAnchor],
		[sendButton.widthAnchor constraintEqualToConstant:50],
		
		[self.inputTextField.trailingAnchor constraintEqualToAnchor:sendButton.leadingAnchor constant:-10],
	]];
}

- (void)viewDidAppear:(BOOL)animated {
	[super viewDidAppear:animated];
	[self.inputTextField becomeFirstResponder];
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)configurePasswordToggleButton {
	self.togglePasswordButton = [UIButton buttonWithType:UIButtonTypeSystem];
	self.togglePasswordButton.frame = CGRectMake(0.0, 0.0, 32.0, 32.0);
	[self.togglePasswordButton addTarget:self action:@selector(togglePasswordVisibility) forControlEvents:UIControlEventTouchUpInside];
	self.togglePasswordButton.accessibilityLabel = @"Toggle password visibility";
	[self updatePasswordToggleButtonAppearance];
	
	self.inputTextField.rightView = self.togglePasswordButton;
	self.inputTextField.rightViewMode = UITextFieldViewModeAlways;
}

- (void)updatePasswordToggleButtonAppearance {
	if (@available(iOS 13.0, *)) {
		NSString *imageName = self.inputTextField.secureTextEntry ? @"eye" : @"eye.slash";
		UIImage *image = [UIImage systemImageNamed:imageName];
		[self.togglePasswordButton setImage:image forState:UIControlStateNormal];
		[self.togglePasswordButton setTitle:nil forState:UIControlStateNormal];
	} else {
		NSString *title = self.inputTextField.secureTextEntry ? @"Show" : @"Hide";
		[self.togglePasswordButton setImage:nil forState:UIControlStateNormal];
		[self.togglePasswordButton setTitle:title forState:UIControlStateNormal];
		self.togglePasswordButton.titleLabel.font = [UIFont systemFontOfSize:12];
	}
}

- (void)togglePasswordVisibility {
	NSString *currentText = self.inputTextField.text ?: @"";
	BOOL wasFirstResponder = self.inputTextField.isFirstResponder;
	
	if (wasFirstResponder) {
		[self.inputTextField resignFirstResponder];
	}
	
	self.inputTextField.secureTextEntry = !self.inputTextField.secureTextEntry;
	self.inputTextField.text = currentText;
	[self updatePasswordToggleButtonAppearance];
	
	if (wasFirstResponder) {
		[self.inputTextField becomeFirstResponder];
	}
}

- (BOOL)textField:(UITextField *)textField shouldChangeCharactersInRange:(NSRange)range replacementString:(NSString *)text {
	NSString *currentText = textField.text ?: @"";
	NSString *updatedText = [currentText stringByReplacingCharactersInRange:range withString:text];
	
	if (updatedText.length <= g_maxLength) {
		return YES;
	}
	
	return NO;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
	[self sendInputTapped];
	return NO;
}

#pragma mark - Keyboard Notifications

- (void)keyboardWillShow:(NSNotification *)notification {
	NSDictionary *userInfo = notification.userInfo;
	NSValue *keyboardFrameValue = userInfo[UIKeyboardFrameEndUserInfoKey];
	CGRect keyboardFrame = [keyboardFrameValue CGRectValue];
	
	NSTimeInterval duration = [userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
	UIViewAnimationOptions curve = [userInfo[UIKeyboardAnimationCurveUserInfoKey] integerValue] << 16;
	
	CGFloat bottomInset = 0.0f;
	if (@available(iOS 11.0, *)) {
		bottomInset = self.view.safeAreaInsets.bottom;
	}
	
	CGFloat keyboardHeight = keyboardFrame.size.height - bottomInset;
	
	self.inputContainerBottomConstraint.constant = -keyboardHeight;
	
	[UIView animateWithDuration:duration delay:0.0 options:curve animations:^{
		// do animation
		[self.view layoutIfNeeded];
	} completion:nil];
}

- (void)keyboardWillHide:(NSNotification *)notification {
	NSDictionary *userInfo = notification.userInfo;
	NSTimeInterval duration = [userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
	UIViewAnimationOptions curve = [userInfo[UIKeyboardAnimationCurveUserInfoKey] integerValue] << 16;
	
	self.inputContainerBottomConstraint.constant = 0;
	
	[UIView animateWithDuration:duration delay:0.0 options:curve animations:^{
		// do animation
		[self.view layoutIfNeeded];
	} completion:nil];
}

#pragma mark - UITextFieldDelegate

- (void)sendInputTapped {
	[self.inputTextField resignFirstResponder];
	[self finishInputAndDismiss];
	
	NSString *text = self.inputTextField.text ?: @"";
	textfield_on_done([text UTF8String]);
}

- (void)textFieldDidChange:(UITextField *)textField {
	NSString *newText = textField.text ?: @"";
	textfield_on_change([newText UTF8String]);
}

/*
 - (void)hideInputView {
 self.inputContainerBottomConstraint.constant = 50;
 
 [UIView animateWithDuration:0.3 animations:^{
 [self.view layoutIfNeeded];
 } completion:^(BOOL finished) {
 self.inputContainerView.hidden = YES;
 self.inputTextField.text = @"";
 }];
 }
 */

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
	// touch int outside
	UITouch *touch = [touches anyObject];
	if (touch.view == self.view) {
		[self finishInputAndDismiss];
		
		NSString *text = self.inputTextField.text ?: @"";
		textfield_on_done([text UTF8String]);
	}
}

- (void)finishInputAndDismiss {
	// hide textField
	[self.inputTextField resignFirstResponder];
	
	[self dismissViewControllerAnimated:YES completion:nil];
}

@end

#endif
