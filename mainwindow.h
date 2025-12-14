#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QMap>

// 哈夫曼树节点结构
struct HuffmanNode {
    QChar ch;           // 字符
    int freq;           // 频率
    HuffmanNode *left;  // 左子节点
    HuffmanNode *right; // 右子节点

    HuffmanNode(QChar c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
    HuffmanNode(int f, HuffmanNode *l, HuffmanNode *r) : ch('\0'), freq(f), left(l), right(r) {}
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStatFreqBtnClicked();
    void onGenerateCodeBtnClicked();
    void onEncodeDecodeBtnClicked();
    void onVerifyBtnClicked();
    void onResetBtnClicked();

private:
    void setupUI();
    void connectSignals();

    // 哈夫曼树相关函数
    void buildHuffmanTree();
    void generateCodes(HuffmanNode *node, const QString &code);
    void deleteHuffmanTree(HuffmanNode *node);
    void updateCodeTable();
    QString encodeText(const QString &text);
    QString decodeText(const QString &encodedText);

    // 字符频率数据
    QMap<QChar, int> m_charFrequency;

    // 哈夫曼编码数据
    QMap<QChar, QString> m_huffmanCodes;
    HuffmanNode *m_huffmanRoot;

    // 文本编辑区
    QTextEdit *m_inputTextEdit;

    // 字符频率显示区
    QTableWidget *m_frequencyTable;

    // 编码表显示区
    QTableWidget *m_codeTable;

    // 编码结果区
    QTextEdit *m_encodeResultEdit;

    // 解码结果区
    QTextEdit *m_decodeResultEdit;

    // 校验区
    QLabel *m_verifyResultLabel;

    // 功能按钮
    QPushButton *m_statFreqBtn;
    QPushButton *m_generateCodeBtn;
    QPushButton *m_encodeDecodeBtn;
    QPushButton *m_verifyBtn;
    QPushButton *m_resetBtn;
};
#endif // MAINWINDOW_H
