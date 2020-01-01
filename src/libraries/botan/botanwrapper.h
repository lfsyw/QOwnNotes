#ifndef BOTANWRAPPER_H
#define BOTANWRAPPER_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <memory>
#include "botan.h"

using namespace Botan;

class BotanWrapper : public QObject
{
    Q_OBJECT
public:
    explicit BotanWrapper(QObject *parent = 0);
    
    /*!
    * Creates a hash
    * @param Data The string to hash
    */
    QString Hash(const QString &Data);

    /*!
    * Creates a hash in hex format
    * @param Data The string to hash
    */
    QString HexHash(const QString &Data);

    /*!
    * Returns a Base64 encoded QString
    * @param Data The string to encode
    */
    QString Encode(const QString &Data);

    /*!
    * Returns a decoded string from a Base64 encoded string
    * @param Data The string to decode
    */
    QString Decode(const QString &Data);

    /*!
    * Returns a Base64 encrypted QString
    * @param Data The string to encypt
    */
    QString Encrypt(const QString &Data);

    /*!
    * Returns a decrypted string from a Base64 encypted string
    * @param Data The string to encypt
    */
    QString Decrypt(const QString &Data);

    /*!
    * Encrypts a file and returns a bool indicating success
    * @param Source The source file
    * @param Destination The destination file
    */
    bool EncryptFile(const QString &Source, const QString &Destination);

    /*!
    * Decrypts a file and returns a bool indicating success
    * @param Source The source file
    * @param Destination The destination file
    */
    bool DecryptFile(const QString &Source, const QString &Destination);

    /*!
    * Sets the Password
    * @param Password The password
    */
    void setPassword(const QString &Password);

    /*!
    * Sets the Salt
    * @param Salt The salt value
    */
    void setSalt(const QString &Salt);

signals:
    
public slots:

private:
    /*!
    * The botan libary initilizer
    */
    //Botan::LibraryInitializer mInit;

    /*!
    * The Salt
    */
    secure_vector<byte> mSalt;

    /*!
    * The password
    */
    QString mPassword;

};

#endif // BOTANWRAPPER_H
