/* This file is part of Clementine.

   Clementine is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Clementine is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Clementine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "lyricfetcher.h"
#include "ultimatelyricsreader.h"

#include <QFutureWatcher>
#include <QtConcurrentRun>
#include <QtDebug>

typedef QList<LyricProvider*> ProviderList;

LyricFetcher::LyricFetcher(NetworkAccessManager* network, QObject* parent)
  : QObject(parent),
    network_(network),
    next_id_(1),
    ultimate_reader_(new UltimateLyricsReader(network))
{
  // Parse the ultimate lyrics xml file in the background
  QFuture<ProviderList> future = QtConcurrent::run(
      ultimate_reader_.get(), &UltimateLyricsReader::Parse,
      QString(":lyrics/ultimate_providers.xml"));
  QFutureWatcher<ProviderList>* watcher = new QFutureWatcher<ProviderList>(this);
  watcher->setFuture(future);
  connect(watcher, SIGNAL(finished()), SLOT(UltimateLyricsParsed()));
}

LyricFetcher::~LyricFetcher() {
}

void LyricFetcher::UltimateLyricsParsed() {
  QFutureWatcher<ProviderList>* watcher =
      static_cast<QFutureWatcher<ProviderList>*>(sender());

  providers_.append(watcher->future().results()[0]);

  watcher->deleteLater();
  ultimate_reader_.reset();
}

int LyricFetcher::SearchAsync(const Song& metadata) {
  const int id = next_id_ ++;

  QtConcurrent::run(this, &LyricFetcher::DoSearch, metadata, id);

  return id;
}

void LyricFetcher::DoSearch(const Song& metadata, int id) {
  foreach (LyricProvider* provider, providers_) {
    qDebug() << "Searching" << metadata.title() << "with" << provider->name();

    LyricProvider::Result result = provider->Search(metadata);
    if (result.valid) {
      qDebug() << "Content" << result.content;
      emit SearchResult(id, true, result.title, result.content);
      //return;
    }
  }

  emit SearchResult(id, false, QString(), QString());
}
