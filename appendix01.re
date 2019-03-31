= ユーザ向けAPIと内部向けAPI

ここからはおまけです。どこかで説明を入れたかったが、適切な場所が無かった話を入れています。まずはユーザ向けAPIと内部向けAPIについてです。

Railsは、ユーザ向けAPI(user-facing API)とinternal API(内部向けAPI)が明確に分かれています。

API doc@<fn>{api_doc}に記載されているAPIだけがユーザ向けAPIで、それ以外のAPIは全てRails内部で使用する内部向けAPI(internal API)になります。
//footnote[api_doc][@<href>{https://api.rubyonrails.org/}]

ユーザ向けAPIと内部向けAPIは、APIの挙動を変更する際の対応が異なります。ユーザ向けAPIはリリース間で非互換になる対応は行いません。非互換になる対応が必要な場合、基本的にはdeprecationサイクル(挙動が変わる旨ユーザに通知してから挙動を変更する)を行うようにしており、バージョンが上がってもいきなり振る舞いが変わる事が無いようにしています。

なお、まれにこのdeprecationサイクルを行わない変更があります。その場合、アップグレードガイド@<fn>{upgrading}に説明を記載するようになっています。ユーザ向けAPIの挙動が変わって、かつ、それがアップグレードガイドに説明が無い場合、バグかガイドへの記載漏れです。
//footnote[deprecation][変更内容的に元の挙動を維持するのが困難な場合、deprecationメッセージ無しに変更してしまう事があります]
//footnote[upgrading][@<href>{https://edgeguides.rubyonrails.org/up_ruby_on_rails.html}]

逆にいうと、内部向けAPIはバージョン間で非互換になる対応が行われます。

内部向けAPIを使うとバージョンアップグレード時の難易度が上がります(前のバージョンで通っていた処理がいきなりエラーで何故か落ちる、特定の引数を渡した時の挙動が変わってる等々)ので、内部向けAPIはRailsアプリケーションでは使用しない事を推奨します。どうしても使用したいAPIが内部向けになっている場合、そのAPIを公開APIにするようRailsにPRを送るか、内部向けAPIを使用している部分をgemにして、Railsアプリケーションとは別に管理しテストしやすくする事をオススメします。

因みに、API docには、リリース済みのgemに基づいて作成された@<href>{https://api.rubyonrails.org/}と、GitHubの最新のコミットに基づいて作成された@<href>{https://edgeapi.rubyonrails.org/}の2つがあります。@<href>{https://api.rubyonrails.org/}は基本的にはRailsがリリースされた時にしか更新されないので、最新の内容を確認したい場合、@<href>{https://edgeapi.rubyonrails.org/}の方でご確認して下さい。

= rspec-rails

rspecでRailsアプリケーションのテストで使用する場合rspec-rails gemを使用するかと思います。このgemで提供されているRailsの機能をテストする為の仕組みは、2章で紹介したクラスたちを使用しているのではなく、それらのクラスで使用しているモジュールを直接使用して実装されています。

そのため、rspec-railsで提供しているテストの為の機能とRailsのテストの為の機能では若干挙動が異なります。また、rspec-railsではRails内部向けのAPIを使用している箇所もあります。そのため、Railsのバージョンをあげたらrspecの挙動がおかしくなった、という事がありえます。ご注意下さい@<fn>{rspec-rails}。
//footnote[rspec-rails][rspe-railsは開発がアクティブなのでRailsの正式リリースより前に対応が行われる事が殆どだと思いますが。]
