//
//  ViewController.m
//  FileParser
//
//  Created by Sergey Orekhov on 25.05.2022.
//

#import "ViewController.h"
#import "CLogReader.hpp"

@interface ViewController () <NSURLSessionDelegate, UITableViewDataSource, UITextFieldDelegate> {
    CLogReader *_logReader;
}

@property (retain, nonatomic) IBOutlet UITextField *urlText;
@property (retain, nonatomic) IBOutlet UITextField *filterText;

@property (retain, nonatomic) IBOutlet UIButton *runBtn;
@property (retain, nonatomic) IBOutlet UITableView *resultsTable;

@property (retain, nonatomic) NSURLSessionDataTask *downloadTask;
@property (retain, atomic) NSMutableArray *foundLines;

- (void)didReceiveLine:(NSString *)line;

@end

class CLogReaderListenerImpl : public CLogReaderListener {
public:
    CLogReaderListenerImpl(ViewController *controller) {
        controller_ = controller;
    }
    
    ~CLogReaderListenerImpl() override {
        controller_ = nil;
    }
    
    void NewLineFound(const char *line) override {
        [controller_ didReceiveLine:[NSString stringWithUTF8String:line]];
    }
    
private:
    ViewController *controller_;
};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    _foundLines = [[NSMutableArray alloc] init];
    _resultsTable.dataSource = self;
    
    _urlText.delegate = self;
    _filterText.delegate = self;
    
    _runBtn.enabled = NO;
    
    _logReader = new CLogReader();
    _logReader->SetListener(new CLogReaderListenerImpl(self));
    
}

- (IBAction)textFieldChanged:(id)sender {
    _runBtn.enabled = _urlText.text.length > 0;
}

- (void)dealloc {
    if (_logReader) {
        _logReader->SetListener(nullptr);
        
        delete _logReader;
        _logReader = nullptr;
    }
    
    [_runBtn release];
    [_resultsTable release];
    [_foundLines release];
    
    [_urlText release];
    [_filterText release];
    [super dealloc];
}

- (IBAction)runBtnTouch:(UIButton *)sender {
    [_urlText resignFirstResponder];
    [_filterText resignFirstResponder];
    
    [_foundLines removeAllObjects];
    [_resultsTable reloadData];
    
    _runBtn.enabled = NO;
    _urlText.enabled = NO;
    _filterText.enabled = NO;
    
    _logReader->SetFilter(_filterText.text.length > 0 ? _filterText.text.UTF8String : nullptr);
    
    NSURLSession *session = [NSURLSession sessionWithConfiguration:[NSURLSessionConfiguration defaultSessionConfiguration]
                                                          delegate:self
                                                     delegateQueue: nil];
    _downloadTask = [session dataTaskWithURL:[NSURL URLWithString:_urlText.text]];
    [_downloadTask resume];
    
    [session release];
}

- (void)didReceiveLine:(NSString *)line {
    [_foundLines addObject:line];
    
    dispatch_sync(dispatch_get_main_queue(), ^{
        [_resultsTable reloadData];
    });
}

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [[event allTouches] anyObject];
    if (touch) {
        if (touch.view != _urlText && touch.view != _filterText) {
            [_urlText resignFirstResponder];
            [_filterText resignFirstResponder];
        }
    }
}

- (void)URLSession:(NSURLSession *)session
          dataTask:(NSURLSessionDataTask *)dataTask
didReceiveResponse:(NSURLResponse *)response
 completionHandler:(void (^)(NSURLSessionResponseDisposition disposition))completionHandler {
    completionHandler(NSURLSessionResponseAllow);
}

- (void)URLSession:(NSURLSession *)session
          dataTask:(NSURLSessionDataTask *)dataTask
    didReceiveData:(NSData *)data {
    if (_logReader) {
        NSString *text = [[NSString alloc] initWithData:data encoding:NSASCIIStringEncoding];
        _logReader->AddSourceBlock([text UTF8String], text.length);
        [text release];
    }
}

- (void)URLSession:(NSURLSession *)session
              task:(NSURLSessionTask *)task
didCompleteWithError:(nullable NSError *)error {
    if (error) {
        NSLog(@"Failed with error: %@", error.description);
    }
    else {
        if (task.state == NSURLSessionTaskStateCompleted) {
        }
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        _runBtn.enabled = YES;
        _urlText.enabled = YES;
        _filterText.enabled = YES;
    });
    
    if (_logReader) {
        _logReader->AddSourceBlock(nullptr, 0);
    }
    
    // write results to file
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *resultsFile = [NSString stringWithFormat:@"%@/%@", paths[0], @"results.log"];
    NSMutableString *result = [[NSMutableString alloc] init];
    [result appendString:@"Found lines:\n"];
    for (NSString *line in _foundLines) {
        [result appendFormat:@"%@\n", line];
    }
    [[result dataUsingEncoding:NSUTF8StringEncoding] writeToFile:resultsFile atomically:NO];
    [result release];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return _foundLines ? [_foundLines count] : 0;
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                                   reuseIdentifier:nil];
    [[cell textLabel] setText:_foundLines[indexPath.row]];
    [[cell textLabel] setNumberOfLines:0];
    [[cell textLabel] setLineBreakMode:NSLineBreakByWordWrapping];
    [[cell textLabel] setFont:[UIFont systemFontOfSize: 14.0]];
    return cell;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField {
    [textField resignFirstResponder];
    return NO;
}

@end
