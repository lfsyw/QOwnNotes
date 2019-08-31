/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#pragma once

#include <QSqlQuery>
#include <QList>
#include <QJsonObject>
#include <QUrl>
#include <QStringList>
#include <QtCore/QDir>

class ScriptInfoJson {
public:
    explicit ScriptInfoJson(const QJsonObject& jsonObject);

    QString name;
    QString identifier;
    QString version;
    QString minAppVersion;
    QString script;
    QString description;
    QStringList richAuthorList;
    QStringList platformList;
    QStringList richPlatformList;
    QStringList resources;
    QString richAuthorText;
    QString richPlatformText;
    bool platformSupported;
    bool appVersionSupported;
};

class Script
{
public:
    static const QString ScriptRepositoryRawContentUrlPrefix;

    explicit Script();

    int getId();
    static bool create(const QString& name, const QString& scriptPath);
    static Script fetch(int id);
    static Script scriptFromQuery(const QSqlQuery& query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const Script &script);
    bool exists();
    bool fillFromQuery(const QSqlQuery& query);
    bool remove();
    bool isFetched();
    static QList<Script> fetchAll(bool enabledOnly = false);
    QString getName();
    QString getScriptPath();
    int getPriority();
    void setName(const QString& text);
    void setScriptPath(const QString& text);
    void setPriority(int value);
    static int countAll();
    bool scriptPathExists();
    void setEnabled(bool value);
    bool getEnabled();
    bool isEnabled();
    static int countEnabled();
    void setIdentifier(const QString& identifier);
    void setInfoJson(const QString& infoJson);
    QString getIdentifier();
    QJsonObject getInfoJsonObject();
    static QString globalScriptRepositoryPath();
    QString scriptRepositoryPath(bool removeRecursively = false);
    bool isScriptFromRepository();
    QUrl remoteScriptUrl();
    QUrl remoteFileUrl(const QString& fileName);
    static bool scriptFromRepositoryExists(const QString& identifier);
    void setSettingsVariablesJson(const QString& json);
    QString getSettingsVariablesJson();
    QJsonObject getSettingsVariablesJsonObject();
    void setSettingsVariablesJson(const QJsonObject& jsonObject);
    QString getScriptDirPath();
    QList<QUrl> remoteFileUrls();
    ScriptInfoJson getScriptInfoJson();
    static Script fetchByIdentifier(const QString& identifier);
    bool refetch();
    bool fillFromId(int id);
    QUrl repositoryInfoJsonUrl();

private:
    int id;
    QString name;
    QString identifier;
    QString infoJson;
    QString settingsVariablesJson;
    QString scriptPath;
    int priority;
    bool enabled;
};
