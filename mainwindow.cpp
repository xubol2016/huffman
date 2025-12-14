#include "mainwindow.h"
#include <QHeaderView>
#include <queue>
#include <vector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_huffmanRoot(nullptr)
{
    setupUI();
    connectSignals();
}

MainWindow::~MainWindow()
{
    deleteHuffmanTree(m_huffmanRoot);
}

void MainWindow::setupUI()
{
    // 设置窗口标题和大小
    setWindowTitle("霍夫曼编码器");
    resize(1200, 800);

    // 创建中央部件
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // 主布局：水平分割
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // ========== 左侧区域 ==========
    QVBoxLayout *leftLayout = new QVBoxLayout();

    // 文本输入区
    QGroupBox *inputGroup = new QGroupBox("文本输入区");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    m_inputTextEdit = new QTextEdit();
    m_inputTextEdit->setPlaceholderText("请输入或粘贴需要编码的文本内容...");
    m_inputTextEdit->setMinimumHeight(150);
    inputLayout->addWidget(m_inputTextEdit);

    // 编码结果区
    QGroupBox *encodeGroup = new QGroupBox("编码结果区");
    QVBoxLayout *encodeLayout = new QVBoxLayout(encodeGroup);
    m_encodeResultEdit = new QTextEdit();
    m_encodeResultEdit->setPlaceholderText("编码后的二进制序列将显示在这里...");
    m_encodeResultEdit->setReadOnly(true);
    m_encodeResultEdit->setMinimumHeight(120);
    encodeLayout->addWidget(m_encodeResultEdit);

    // 解码结果区
    QGroupBox *decodeGroup = new QGroupBox("解码结果区");
    QVBoxLayout *decodeLayout = new QVBoxLayout(decodeGroup);
    m_decodeResultEdit = new QTextEdit();
    m_decodeResultEdit->setPlaceholderText("解码后的文本将显示在这里...");
    m_decodeResultEdit->setReadOnly(true);
    m_decodeResultEdit->setMinimumHeight(120);
    decodeLayout->addWidget(m_decodeResultEdit);

    // 校验区
    QGroupBox *verifyGroup = new QGroupBox("校验区");
    QVBoxLayout *verifyLayout = new QVBoxLayout(verifyGroup);
    m_verifyResultLabel = new QLabel("校验结果：等待校验...");
    m_verifyResultLabel->setAlignment(Qt::AlignCenter);
    m_verifyResultLabel->setMinimumHeight(40);
    m_verifyResultLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    verifyLayout->addWidget(m_verifyResultLabel);

    leftLayout->addWidget(inputGroup, 2);
    leftLayout->addWidget(encodeGroup, 2);
    leftLayout->addWidget(decodeGroup, 2);
    leftLayout->addWidget(verifyGroup, 1);

    // ========== 右侧区域 ==========
    QVBoxLayout *rightLayout = new QVBoxLayout();

    // 字符频率显示区
    QGroupBox *freqGroup = new QGroupBox("字符频率显示区");
    QVBoxLayout *freqLayout = new QVBoxLayout(freqGroup);
    m_frequencyTable = new QTableWidget();
    m_frequencyTable->setColumnCount(3);
    m_frequencyTable->setHorizontalHeaderLabels(QStringList() << "字符" << "出现次数" << "频率");
    m_frequencyTable->horizontalHeader()->setStretchLastSection(true);
    m_frequencyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_frequencyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_frequencyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    freqLayout->addWidget(m_frequencyTable);

    // 编码表显示区
    QGroupBox *codeGroup = new QGroupBox("编码表显示区");
    QVBoxLayout *codeLayout = new QVBoxLayout(codeGroup);
    m_codeTable = new QTableWidget();
    m_codeTable->setColumnCount(2);
    m_codeTable->setHorizontalHeaderLabels(QStringList() << "字符" << "哈夫曼编码");
    m_codeTable->horizontalHeader()->setStretchLastSection(true);
    m_codeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_codeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_codeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    codeLayout->addWidget(m_codeTable);

    rightLayout->addWidget(freqGroup, 1);
    rightLayout->addWidget(codeGroup, 1);

    // ========== 按钮区域 ==========
    QGroupBox *btnGroup = new QGroupBox("操作");
    QVBoxLayout *btnLayout = new QVBoxLayout(btnGroup);

    m_statFreqBtn = new QPushButton("统计频率");
    m_generateCodeBtn = new QPushButton("生成编码");
    m_encodeDecodeBtn = new QPushButton("执行编解码");
    m_verifyBtn = new QPushButton("校验");
    m_resetBtn = new QPushButton("重置");

    // 设置按钮最小高度
    m_statFreqBtn->setMinimumHeight(35);
    m_generateCodeBtn->setMinimumHeight(35);
    m_encodeDecodeBtn->setMinimumHeight(35);
    m_verifyBtn->setMinimumHeight(35);
    m_resetBtn->setMinimumHeight(35);

    btnLayout->addWidget(m_statFreqBtn);
    btnLayout->addWidget(m_generateCodeBtn);
    btnLayout->addWidget(m_encodeDecodeBtn);
    btnLayout->addWidget(m_verifyBtn);
    btnLayout->addSpacing(20);
    btnLayout->addWidget(m_resetBtn);
    btnLayout->addStretch();

    rightLayout->addWidget(btnGroup);

    // ========== 组合布局 ==========
    QWidget *leftWidget = new QWidget();
    leftWidget->setLayout(leftLayout);

    QWidget *rightWidget = new QWidget();
    rightWidget->setLayout(rightLayout);

    // 使用QSplitter实现可调整大小的分割
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftWidget);
    splitter->addWidget(rightWidget);
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);

    mainLayout->addWidget(splitter);
}

void MainWindow::connectSignals()
{
    connect(m_statFreqBtn, &QPushButton::clicked, this, &MainWindow::onStatFreqBtnClicked);
    connect(m_generateCodeBtn, &QPushButton::clicked, this, &MainWindow::onGenerateCodeBtnClicked);
    connect(m_encodeDecodeBtn, &QPushButton::clicked, this, &MainWindow::onEncodeDecodeBtnClicked);
    connect(m_verifyBtn, &QPushButton::clicked, this, &MainWindow::onVerifyBtnClicked);
    connect(m_resetBtn, &QPushButton::clicked, this, &MainWindow::onResetBtnClicked);
}

void MainWindow::onStatFreqBtnClicked()
{
    // 获取输入文本
    QString text = m_inputTextEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    // 清空之前的统计数据
    m_charFrequency.clear();

    // 统计每个字符的出现次数
    for (const QChar &ch : text) {
        m_charFrequency[ch]++;
    }

    // 计算总字符数
    int totalCount = text.length();

    // 更新频率表格显示
    m_frequencyTable->setRowCount(m_charFrequency.size());

    int row = 0;
    for (auto it = m_charFrequency.begin(); it != m_charFrequency.end(); ++it) {
        QChar ch = it.key();
        int count = it.value();
        double frequency = static_cast<double>(count) / totalCount;

        // 字符显示处理（特殊字符显示为转义形式）
        QString charDisplay;
        if (ch == '\n') {
            charDisplay = "\\n";
        } else if (ch == '\r') {
            charDisplay = "\\r";
        } else if (ch == '\t') {
            charDisplay = "\\t";
        } else if (ch == ' ') {
            charDisplay = "(空格)";
        } else {
            charDisplay = ch;
        }

        // 设置表格内容
        m_frequencyTable->setItem(row, 0, new QTableWidgetItem(charDisplay));
        m_frequencyTable->setItem(row, 1, new QTableWidgetItem(QString::number(count)));
        m_frequencyTable->setItem(row, 2, new QTableWidgetItem(QString::number(frequency, 'f', 4)));

        // 居中对齐
        m_frequencyTable->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        m_frequencyTable->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        m_frequencyTable->item(row, 2)->setTextAlignment(Qt::AlignCenter);

        row++;
    }
}

void MainWindow::onGenerateCodeBtnClicked()
{
    // 检查是否已统计频率
    if (m_charFrequency.isEmpty()) {
        return;
    }

    // 构建哈夫曼树
    buildHuffmanTree();

    // 生成哈夫曼编码
    m_huffmanCodes.clear();
    if (m_huffmanRoot) {
        generateCodes(m_huffmanRoot, "");
    }

    // 更新编码表显示
    updateCodeTable();
}

void MainWindow::onEncodeDecodeBtnClicked()
{
    // 检查是否已生成编码
    if (m_huffmanCodes.isEmpty()) {
        return;
    }

    // 获取输入文本并编码
    QString text = m_inputTextEdit->toPlainText();
    if (text.isEmpty()) {
        return;
    }

    // 编码
    QString encodedText = encodeText(text);
    m_encodeResultEdit->setPlainText(encodedText);

    // 解码
    QString decodedText = decodeText(encodedText);
    m_decodeResultEdit->setPlainText(decodedText);
}

void MainWindow::buildHuffmanTree()
{
    // 释放之前的树
    deleteHuffmanTree(m_huffmanRoot);
    m_huffmanRoot = nullptr;

    if (m_charFrequency.isEmpty()) {
        return;
    }

    // 使用优先队列（最小堆）构建哈夫曼树
    auto cmp = [](HuffmanNode *a, HuffmanNode *b) {
        return a->freq > b->freq;
    };
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype(cmp)> pq(cmp);

    // 为每个字符创建叶子节点
    for (auto it = m_charFrequency.begin(); it != m_charFrequency.end(); ++it) {
        pq.push(new HuffmanNode(it.key(), it.value()));
    }

    // 构建哈夫曼树
    while (pq.size() > 1) {
        HuffmanNode *left = pq.top();
        pq.pop();
        HuffmanNode *right = pq.top();
        pq.pop();

        // 创建新的内部节点
        HuffmanNode *parent = new HuffmanNode(left->freq + right->freq, left, right);
        pq.push(parent);
    }

    // 根节点
    if (!pq.empty()) {
        m_huffmanRoot = pq.top();
    }
}

void MainWindow::generateCodes(HuffmanNode *node, const QString &code)
{
    if (!node) {
        return;
    }

    // 叶子节点，存储编码
    if (!node->left && !node->right) {
        // 处理只有一个字符的特殊情况
        m_huffmanCodes[node->ch] = code.isEmpty() ? "0" : code;
        return;
    }

    // 递归生成左右子树的编码
    generateCodes(node->left, code + "0");
    generateCodes(node->right, code + "1");
}

void MainWindow::deleteHuffmanTree(HuffmanNode *node)
{
    if (!node) {
        return;
    }
    deleteHuffmanTree(node->left);
    deleteHuffmanTree(node->right);
    delete node;
}

void MainWindow::updateCodeTable()
{
    m_codeTable->setRowCount(m_huffmanCodes.size());

    int row = 0;
    for (auto it = m_huffmanCodes.begin(); it != m_huffmanCodes.end(); ++it) {
        QChar ch = it.key();
        QString code = it.value();

        // 字符显示处理
        QString charDisplay;
        if (ch == '\n') {
            charDisplay = "\\n";
        } else if (ch == '\r') {
            charDisplay = "\\r";
        } else if (ch == '\t') {
            charDisplay = "\\t";
        } else if (ch == ' ') {
            charDisplay = "(空格)";
        } else {
            charDisplay = ch;
        }

        m_codeTable->setItem(row, 0, new QTableWidgetItem(charDisplay));
        m_codeTable->setItem(row, 1, new QTableWidgetItem(code));

        m_codeTable->item(row, 0)->setTextAlignment(Qt::AlignCenter);
        m_codeTable->item(row, 1)->setTextAlignment(Qt::AlignCenter);

        row++;
    }
}

QString MainWindow::encodeText(const QString &text)
{
    QString result;
    for (const QChar &ch : text) {
        if (m_huffmanCodes.contains(ch)) {
            result += m_huffmanCodes[ch];
        }
    }
    return result;
}

QString MainWindow::decodeText(const QString &encodedText)
{
    if (!m_huffmanRoot || encodedText.isEmpty()) {
        return QString();
    }

    QString result;
    HuffmanNode *current = m_huffmanRoot;

    // 处理只有一个字符的特殊情况
    if (!m_huffmanRoot->left && !m_huffmanRoot->right) {
        for (int i = 0; i < encodedText.length(); ++i) {
            result += m_huffmanRoot->ch;
        }
        return result;
    }

    // 遍历编码串进行解码
    for (const QChar &bit : encodedText) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        // 到达叶子节点，输出字符
        if (current && !current->left && !current->right) {
            result += current->ch;
            current = m_huffmanRoot;
        }
    }

    return result;
}

void MainWindow::onVerifyBtnClicked()
{
    // 获取原始文本和解码文本
    QString originalText = m_inputTextEdit->toPlainText();
    QString decodedText = m_decodeResultEdit->toPlainText();

    // 检查是否有内容可供校验
    if (originalText.isEmpty() || decodedText.isEmpty()) {
        m_verifyResultLabel->setText("校验结果：无内容可校验");
        m_verifyResultLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: gray;");
        return;
    }

    // 对比原始文本和解码文本
    if (originalText == decodedText) {
        m_verifyResultLabel->setText("校验结果：一致");
        m_verifyResultLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: green;");
    } else {
        m_verifyResultLabel->setText("校验结果：不一致");
        m_verifyResultLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: red;");
    }
}

void MainWindow::onResetBtnClicked()
{
    // 清空所有输入和显示区域
    m_inputTextEdit->clear();
    m_encodeResultEdit->clear();
    m_decodeResultEdit->clear();

    // 清空表格
    m_frequencyTable->setRowCount(0);
    m_codeTable->setRowCount(0);

    // 重置校验结果
    m_verifyResultLabel->setText("校验结果：等待校验...");
    m_verifyResultLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: black;");

    // 清空数据
    m_charFrequency.clear();
    m_huffmanCodes.clear();

    // 释放哈夫曼树
    deleteHuffmanTree(m_huffmanRoot);
    m_huffmanRoot = nullptr;
}
