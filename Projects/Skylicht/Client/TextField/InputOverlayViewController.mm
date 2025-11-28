#ifdef IOS

#import "InputOverlayViewController.h"
#include <string>

std::string g_defaultText;
int g_requestHeight = 50;
int g_maxLength = 64;

void textfield_show(const char* text, int maxLength, int height)
{
	g_defaultText = text;
	g_requestHeight = height;
	g_maxLength = maxLength;
	
	InputOverlayViewController *inputVC = [[InputOverlayViewController alloc] init];
	inputVC.modalPresentationStyle = UIModalPresentationOverFullScreen;
	
	[[[[UIApplication sharedApplication] keyWindow] rootViewController] presentViewController:inputVC animated:YES completion:nil];
}

void textfield_on_change(const char* text);
void textfield_on_done(const char* text);

@interface InputOverlayViewController () <UITextViewDelegate>

@property (nonatomic, strong) UITextView *inputTextView;
@property (nonatomic, strong) UIView *inputContainerView;
@property (nonatomic, strong) NSLayoutConstraint *inputContainerBottomConstraint;

@end

@implementation InputOverlayViewController

- (void)viewDidLoad {
	[super viewDidLoad];
	
	[self setupCustomInputView];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillShow:)
												 name:UIKeyboardWillShowNotification
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(keyboardWillHide:)
												 name:UIKeyboardWillHideNotification
											   object:nil];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(textViewDidChangeNotification:)
												 name:UITextViewTextDidChangeNotification
											   object:self.inputTextView];
}

- (void)setupCustomInputView {
	UILayoutGuide *safeAreaGuide;
	if (@available(iOS 11.0, *)) {
		safeAreaGuide = self.view.safeAreaLayoutGuide;
	} else {
		safeAreaGuide = (UILayoutGuide *)self.view;
	}
	
	// 1. Container
	self.inputContainerView = [[UIView alloc] init];
	self.inputContainerView.backgroundColor = [UIColor colorWithWhite:0.95 alpha:1.0];
	self.inputContainerView.translatesAutoresizingMaskIntoConstraints = NO;
	[self.view addSubview:self.inputContainerView];
	
	// 2. UITextView
	self.inputTextView = [[UITextView alloc] init];
	self.inputTextView.translatesAutoresizingMaskIntoConstraints = NO;
	self.inputTextView.layer.cornerRadius = 5.0;
	self.inputTextView.layer.borderWidth = 1.0;
	self.inputTextView.layer.borderColor = [UIColor lightGrayColor].CGColor;
	self.inputTextView.font = [UIFont systemFontOfSize:16];
	self.inputTextView.delegate = self;
	self.inputTextView.text = [NSString stringWithUTF8String:g_defaultText.c_str()];
	[self.inputContainerView addSubview:self.inputTextView];
	
	// 3. UIButton
	UIButton *sendButton = [UIButton buttonWithType:UIButtonTypeSystem];
	[sendButton setTitle:@"Done" forState:UIControlStateNormal];
	sendButton.translatesAutoresizingMaskIntoConstraints = NO;
	[sendButton addTarget:self action:@selector(sendInputTapped) forControlEvents:UIControlEventTouchUpInside];
	[self.inputContainerView addSubview:sendButton];
	
	// 4.  Auto Layout Constraints
	self.inputContainerBottomConstraint = [self.inputContainerView.bottomAnchor constraintEqualToAnchor:self.view.bottomAnchor constant:0];
	
	[NSLayoutConstraint activateConstraints:@[
		[self.inputContainerView.leadingAnchor constraintEqualToAnchor:safeAreaGuide.leadingAnchor],
		[self.inputContainerView.trailingAnchor constraintEqualToAnchor:safeAreaGuide.trailingAnchor],
		[self.inputContainerView.heightAnchor constraintEqualToConstant:g_requestHeight],
		self.inputContainerBottomConstraint,
		
		[self.inputTextView.topAnchor constraintEqualToAnchor:self.inputContainerView.topAnchor constant:5],
		[self.inputTextView.bottomAnchor constraintEqualToAnchor:self.inputContainerView.bottomAnchor constant:-5],
		[self.inputTextView.leadingAnchor constraintEqualToAnchor:self.inputContainerView.leadingAnchor constant:10],
		
		[sendButton.trailingAnchor constraintEqualToAnchor:self.inputContainerView.trailingAnchor constant:-10],
		[sendButton.centerYAnchor constraintEqualToAnchor:self.inputTextView.centerYAnchor],
		[sendButton.widthAnchor constraintEqualToConstant:50],
		
		[self.inputTextView.trailingAnchor constraintEqualToAnchor:sendButton.leadingAnchor constant:-10],
	]];
}

- (void)viewDidAppear:(BOOL)animated {
	[super viewDidAppear:animated];
	[self.inputTextView becomeFirstResponder];
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)textView:(UITextView *)textView shouldChangeTextInRange:(NSRange)range replacementText:(NSString *)text {
	
	NSString *currentText = textView.text;
	NSString *updatedText = [currentText stringByReplacingCharactersInRange:range withString:text];
	
	if (updatedText.length <= g_maxLength) {
		return YES;
	}
	
	return NO;
}

#pragma mark - Keyboard Notifications

- (void)keyboardWillShow:(NSNotification *)notification {
	NSDictionary *userInfo = notification.userInfo;
	NSValue *keyboardFrameValue = userInfo[UIKeyboardFrameEndUserInfoKey];
	CGRect keyboardFrame = [keyboardFrameValue CGRectValue];
	
	NSTimeInterval duration = [userInfo[UIKeyboardAnimationDurationUserInfoKey] doubleValue];
	UIViewAnimationOptions curve = [userInfo[UIKeyboardAnimationCurveUserInfoKey] integerValue] << 16;
	
	CGFloat keyboardHeight = keyboardFrame.size.height - self.view.safeAreaInsets.bottom;
	
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

#pragma mark - UITextViewDelegate

- (void)sendInputTapped {
	[self.inputTextView resignFirstResponder];
	[self finishInputAndDismiss];
	
	NSString *text = self.inputTextView.text;
	textfield_on_done([text UTF8String]);
}

- (void)textViewDidChangeNotification:(NSNotification *)notification {
	if (notification.object == self.inputTextView) {
		NSString *newText = self.inputTextView.text;
		textfield_on_change([newText UTF8String]);
	}
}

/*
 - (void)hideInputView {
 self.inputContainerBottomConstraint.constant = 50;
 
 [UIView animateWithDuration:0.3 animations:^{
 [self.view layoutIfNeeded];
 } completion:^(BOOL finished) {
 self.inputContainerView.hidden = YES;
 self.inputTextView.text = @"";
 }];
 }
 */

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
	// touch int outside
	UITouch *touch = [touches anyObject];
	if (touch.view == self.view) {
		[self finishInputAndDismiss];
		
		NSString *text = self.inputTextView.text;
		textfield_on_done([text UTF8String]);
	}
}

- (void)finishInputAndDismiss {
	// hide textView
	[self.inputTextView resignFirstResponder];
	
	[self dismissViewControllerAnimated:YES completion:nil];
}

@end

#endif