= user-facing APIとinternal API

Railsはユーザ向けAPI(user-facing API)とinternal APIが明確に分かれています。

API docに記載されているAPIだけがユーザ向けAPIで、それ以外のAPIは全てRails内部で使用するAPI(internal API)になります。
//footnote[api_doc][@<href>{https://api.rubyonrails.org/}]

ユーザ向けAPIと内部向けAPIは、APIの挙動を変更する際の対応が異なります。ユーザ向けAPIはリリース間で非互換になる対応は行いません。非互換になる対応が必要な場合、基本的にはdeprecationサイクル(挙動が変わる旨ユーザに通知する)を一度行うようにしており、バージョンが上がってもいきなり振る舞いが変わらないようにしています。

なお、まれにこのdeprecationサイクルを行わない変更があります。その場合、アップグレードガイド@<fn>{upgrading}に説明を記載するようになっています。ユーザ向けAPIの挙動が変わって、かつ、それがアップグレードガイドに説明が無い場合、バグかガイドへの記載もれです。
//footnote[deprecation][変更内容的に元の挙動を維持するのが困難な場合、deprecationメッセージ無しに変更してしまう事もあります]
//footnote[upgrading][@<href>{https://edgeguides.rubyonrails.org/up_ruby_on_rails.html}]

= rspec-rails
