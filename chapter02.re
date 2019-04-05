= テストの為のクラスたち

本章では、Railsが提供しているテスト用のクラスについて説明します。

== クラスの一覧

Railsでは、テスト用のクラスとして次のクラスを提供しています。

//table[test_classes][Railsが提供しているテスト用のクラス]{
クラス名
-------------------------------------------------------------
ActiveSupport::TestCase
ActionMailer::TestCase
ActionView::TestCase
ActionController::TestCase
ActionDispatch::IntegrationTest
ActionDispatch::SystemTestCase
ActiveJob::TestCase
ActionCable::TestCase
ActionCable::Connection::TestCase
ActionCable::Channel::TestCase
ActionMailbox::TestCase
Rails::Generators::TestCase
//}

基本的には各ライブラリ毎にクラスが提供されています。そのため、ライブラリを使用した機能のテストをしたい場合、それぞれに提供されているテストを使えば良いようになっています。

なお、Active RecordやActive Modelなどの一部ライブラリでは専用のクラスは提供されていません。これはActiveSupport::TestCaseを使用すれば十分で、特別な機能を提供する必要が無い、と判断されている為です。専用のクラスが無い場合は、ActiveSupport::TestCaseを使用するようにして下さい。

== ActiveSupport::TestCase

前章でも述べた通り、Railsが提供しているテストクラスのベースになっているクラスです。各テストクラス共通で使用したい機能がある場合、このクラスに追加するようになっています。

例えば、@<code>{assert_not}等の@<code>{assert_not_x}で始まるメソッドはActiveSupport::TestCaseに定義されており、全てのテスト用のクラスで使用出来るようになっています。

他にも、@<code>{travel}、@<code>{travel_to}という時間に関する処理を行う為のヘルパーメソッドが実装されているActiveSupport::Testing::TimeHelpersというモジュールがあります。このモジュールはActiveSupport::TestCaseでincludeされており、全てのテストクラスで使用出来るようになっています。

//list[travel_to][travel_to]{
Time.current # => Wed, 27 Mar 2019 11:56:31 JST +09:00

# 1日前に移動
travel 1.day do
  Time.current # => Tue, 26 Mar 2019 11:56:35 JST +09:00
end

Time.current # => Wed, 27 Mar 2019 11:56:38 JST +09:00
//}

ActiveSupport::TestCaseはRails自体のテストでも使用されています。そのため、Rails自体のテストをする為に必要なメソッドもこのクラスで使用出来るよう対応されています。

例えば、何か既存の機能をdeprecateにした場合に、その機能がdeprecateになっている事を確認する為の@<code>{assert_deprecated}というアサーションがあります。

//list[assert_deprecated][assert_deprecated]{
# update_attributesメソッドを使用するとdeprecateメッセージが出る
topic = Topic.find(1)
assert_deprecated do
  topic.update_attributes("title" => "The First Topic Updated")
end
//}

他にも、標準入出力を抑止したり、モック処理用のメソッドが定義されています。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActiveSupport/TestCase.html}をご参照下さい。

== ActionMailer::TestCase

メール送信機能のテスト用クラスです。

当然のことですが、テストで実際にメールを送信するわけにはいきません。ActionMailer::TestCaseでは、メール送信処理が実行されても実際のメールの送信は行わず、代わりに送信処理が実行されたメールを内部で保持し参照出来るようにしています。合わせて、送信処理が呼ばれた(または呼ばれていない)事を確認する為のアサーション等を提供しています。

//list[actionmailer_testcase][ActionMailer::TestCase]{
class UserMailerTest < ActionMailer::TestCase
  test "invite friend" do
    # invite_friendを実行したら招待用のメールが送信される
    assert_emails 1 do
      User.invite(email: 'friend@example.com')
    end
  end
end
//}

@<code>{assert_emails}は同期処理(@<code>{deliver_now})、非同期処理(@<code>{deliver_later})どちらで送信されたメールもチェックの対象になります。非同期で送信されたメールだけチェックしたい場合は、@<code>{assert_enqueued_emails}を使用する必要があります。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionMailer/TestCase.html}をご参照下さい。

== ActiveJob::TestCase

バックグランドジョブ機能のテスト用クラスです。

メールと異なり、バックグランドジョブはテストで実際に実行しても問題無い事が多いでしょう。しかし例えば、「1時間後に実行されるジョブ」があった場合、テストで実際に1時間待つ訳にはいきません。
ActiveJob::TestCaseでは、ジョブの登録処理が行われたらそのジョブを内部で保持し、どのようなジョブが登録されたかを確認出来るようにしています。その登録された内容を確認する為のアサーションも提供されています。

//list[activejob_testcase][ActiveJob::TestCase]{
class LoggingJobTest < ActiveJob::TestCase
  test "withdrawal" do
    user = User.last
    # ユーザが退会したらLoggingJobが登録される
    assert_enqueued_with(job: LoggingJob) do
      user.withdrawal
    end
  end
end
//}

アサーションではジョブに指定された引数をチェックしたり、登録されたジョブを実行したりする事も出来るようになっています

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActiveJob/TestCase.html}をご参照下さい。

== ActionView::TestCase

名前からAction Viewのテンプレートに関するテスト用のクラスかと推測されるかと思うのですが、実際はちょっと異なりview helperのテスト用クラスです。

ActionView::TestCaseではview helperを使用する為に必要なcontrollerやviewの生成処理を行い、テスト単体でview helperメソッドが使用出来るようにしています。

//list[action_view_test_case][ActionView::TestCase]{
module UsersHelper
  def link_to_user(user)
    link_to "#{user.first_name} #{user.last_name}", user
  end
end

class UsersHelperTest < ActionView::TestCase
  test "link_to_user returns link with user name" do
    user = User.find_by(first_name: "Yamada", last_name: "Taro")
    assert_dom_equal %{<a href="/user/#{user.id}">Yamada Taro</a>},
      link_to_user(user)
  end
end
//}

しかし、Rails 4.2からhelperのテストはそもそも生成されなくなり、このクラスが使用される事は無くなりました。これはview helper単体でのテストはあまり意味が無いのでは、という意見によるものです@<fn>{helper_test}。そのため、クラス自体は残ったはいますが、目にする事はあまりないかと思います。
//footnote[helper_test][インテグレーションテストやシステムテスト等でビューのテストとしてやるのが良いのでは、という意見が多いです。]

== ActionController::TestCase

コントローラのテスト用クラスです。特定のコントローラのメソッドに対して、HTTPリクエストの送信及びレスポンスの確認ができるようになっています。

//list[action_controller_test_case][ActionController::TestCase]{
class UsersControllerTest < ActionController::TestCase
  test "should get index" do
    get :index
    assert_response :success
  end

  test "should create user" do
    assert_difference('User.count') do
      post(:create, params:
        { user: { email: @user.email, name: @user.name } })
    end

    assert_redirected_to user_url(User.last)
  end
end
//}

同様にコントローラのテストを行う為のクラスとしてActionDispatch::IntegrationTestがあります。

ActionDispatch::IntegrationTestだとルーティングもセットでテストが出来る@<fn>{routing}、HTTPリクエストがより実際のリクエストに近い形で送信される等のメリットがあるのですが、実行はActionController::TestCaseの方が高速だった為、コントローラのテストには長らくActionController::TestCaseが使われるようになっていました。
//footnote[routing][ActionController::TestCaseは送信先にコントローラのアクション名を指定する為、ルーティングのテストは出来なかったのでした。]

しかし、Rails 5.0でActionDispatch::IntegrationTestのパフォーマンスが改善され、実行速度の差は大分縮まりました。それにより、コントローラのテストでもActionDispatch::IntegrationTestが使用される事が推奨されるようになり、scaffoldで生成するコントローラのテストでもActionDispatch::IntegrationTestが使用されるようになりました。

因みに、その対応の際にActionController::TestCaseはgemに切り出してRails本体から削除するという話があったのですが、何だかんだまだコードRails本体に残ったままになっています。とはいえ、機能追加等が行われる事は(恐らく)無いので、新規に追加するテストについてはActionDispatch::IntegrationTestを使用する事をおすすめします。

== ActionDispatch::IntegrationTest

コントローラとルーティングのテスト用クラスです。ActionController::TestCaseと異なり、HTTPリクエスト先に任意のパスを指定出来し、ルーティングについても確認出来るようになっています。

//list[integration_test][ActionDispatch::IntegrationTest]{
class UsersControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get users_url
    assert_response :success
  end

  test "should create user" do
    assert_difference('User.count') do
      post users_url, params:
        { user: { email: @user.email, name: @user.name } }
    end

    assert_redirected_to user_url(User.last)
  end
end
//}

ActionDispatch::IntegrationTestではルーティングを確認する為のアサーションが提供されています。

//list[route_assertions][route assertions]{
assert_routing '/home', controller: 'home', action: 'index'
assert_routing 'controller/action/9', {id: "9", item: "square"},
  { controller: "controller", action: "action"}, {}, {item: "square" }

assert_recognizes({controller: 'items', action: 'list'}, 'items/list')
//}

また、ActionDispatch::IntegrationTestはRails 6.0から自動的にActionMailer::TestCase、ActiveJob::TestCaseで使用されているヘルパーモジュール(ActionMailer::TestHelper、ActiveJob::TestHelperを自動的にインクルードするようになりました。これにより、ActionDispatch::IntegrationTest内で行ったメールの送信、バックグランドジョブの登録についてもテスト出来るようになっています。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionDispatch/IntegrationTest.html}をご参照下さい。

== ActionDispatch::SystemTestCase

ActionDispatch::SystemTestCaseはシステムテストの為のクラスです。Capybara@<fn>{capybara}を使用して簡単にブラウザを使用したテストを書けるようになっています。
//footnote[capybara][@<href>{https://github.com/teamcapybara/capybara}]

//list[system_test_example][system test]{
class UsersTest < ApplicationSystemTestCase
  test "creating a User" do
    visit users_url
    click_on "New User"

    fill_in "Email", with: @user.email
    fill_in "Name", with: @user.name
    click_on "Create User"

    assert_text "User was successfully created"
    click_on "Back"
  end
end
//}

ActionDispatch::SystemTestCaseはActionDispatch::IntegrationTestを継承しており、ActionDispatch::IntegrationTest + Capybaraのラッパー的な機能を提供しています。

Capybaraで使用する為のドライバーはActionDispatch::SystemTestCaseで実装されており、ユーザはそのドライバーを指定する為のメソッドを使用すれば、Capybaraの設定を意識する事なくブラウザを指定する事ができるようになっています。

//list[driven_by][system testでブラウザを指定]{
class ApplicationSystemTestCase < ActionDispatch::SystemTestCase
  # テストでヘッドレスChromeを使用する
  driven_by :selenium, using: :headless_chrome, screen_size: [1400, 1400]
end
//}

Capybaraのラッパー以外の機能としては、スクリーンショットの取得機能があります。任意のタイミングでの取得は勿論、テスト失敗時に自動でスクリーンショットの取得を行ってくれるようになっています。なお、テスト失敗時のスクリーンショットの表示は、Rails 6.0だとスクリーンショットのファイル名のみです。ターミナル上でスクリーンショットを直接表示したい場合、`RAILS_SYSTEM_TESTING_SCREENSHOT`に適切な値を指定する必要があります。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionDispatch/SystemTestCase.html}をご参照下さい。

== ActionCable::TestCase

Rails 6.0から追加されたAction Cableのテスト用クラスです。Action Cableが追加された当初、Action Cableのテスト用の機能はRails本体にありませんでした。これは、Action Cableに関するテストはブラウザを使用して行うテスト(現在のシステムテスト)で確認した方が正確で、単体でテストは行う必要は無いのでは、という意見があった為です。

しかし、API-onlyアプリケーションでもAction Cableを使う、というケースが出てきました。API-onlyアプリケーションだとシステムテストは使用出来ない為、専用のテストの仕組みがあった方が良いよね、という声が強まり、晴れてRails本体に機能が追加されました。

なお、元々action-cable-testing@<fn>{action-cable-testing}というgemがあり、その機能をRails本体にインポート@<fn>{import-gem}した形になります。その為、Rails 6.0より前でAction Cableのテストを行いたい場合、action-cable-testingを使用すれば、同等のテストが出来るようになっています。
//footnote[action-cable-testing][@<href>{https://github.com/palkan/action-cable-testing}]
//footnote[import-gem][Rails本体へのインポート処理を行ったのはaction-cable-testingの作者本人です。元々Rails本体にPRを出していたのですが、中々マージされなかった為gemにされたようです。]

ActionCable::TestCaseではテスト用のadapterを使用しブロードキャストの管理を行うようになっていて、メッセージが送信された/されてない等をテスト出来るようになっています。

//list[actioncable_testcase][ActionCable::TestCase]{
class ChartRoomTest < ActionCable::TestCase
  test "broadcast message" do
    assert_broadcasts('messages', 1) do
      ActionCable.server.broadcast 'messages', { text: 'hello' }
    end
  end
end
//}

ActionCable::TestCaseはブロードキャストに関する処理のみ提供しており、コネクション、チャンネルに関するテストは、後述するActionCable::Connection::TestCase、ActionCable::Channel::TestCaseをそれぞれ使用する必要があります。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionCable/TestHelper.html}をご参照下さい。

== ActionCable::Connection::TestCase

Action Cableのコネクションに関するテストの為のクラスです。接続処理の為のヘルパーメソッド(@<code>{connect})や、接続に失敗した事を確認する為のアサーション(@<code>{assert_reject_connection})が提供されています。

//list[connect_testcase][ActionCable::Connection::TestCase]{
class ApplicationCable::ConnectionTest < ActionCable::Connection::TestCase
  # 適切なcookieが設定されていれば接続出来る
  test "connects with proper cookie" do
    cookies["user_id"] = users(:john).id

    connect

    assert_equal users(:john).id, connection.user.id
  end

  # 適切なcookieが設定されていない場合接続エラーになる
  test "rejects connection without proper cookie" do
    assert_reject_connection { connect }
  end
end
//}

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionCable/Connection/TestCase.html}をご参照下さい。

== ActionCable::Channel::TestCase

Action Cableのチャンネルのテストの為のクラスです。チャンネルに対してのsubscriptionの作成処理や、streamが正しく開始されていることを確認する為のアサーションを提供しています。

//list[channel_testcase][ActionCable::Channel::TestCase]{
class ChatChannelTest < ActionCable::Channel::TestCase
  test "subscribes and stream for room" do
    # "room"に対するsubscriptionを作成
    subscribe room: "15"

    # "room" 15に対するstreamが開始されていること
    assert subscription.confirmed?
    assert_has_stream "chat_15"
  end
end
//}

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionCable/Channel/TestCase.html}をご参照下さい。

== ActionMailbox::TestCase

Rails 6.0で追加されたAction Mailboxのテストの為のクラスです。Action Mailboxは、メール受信処理の為のライブラリです。メールの受信 -> メールの内容に応じた各種処理の実施 -> 処理が終わったメールの削除等の機能を提供しています。

「メールの内容に応じた各種処理」は、Mailboxというクラスに定義します。

//list[mailbox][Mailbox]{
class InboxMailbox < ApplicationMailbox
  def process
    # メールの内容をDBに保存
    ReceiveMail.create!(
      from: mail.from.first, to: mail.to.first,
      subject: mail.subject, body: mail.body)

    user = User.find_by(email: mail.to.first)
    # ユーザにメールを通知
    user.notify_email(mail) if user
  end
end
//}

このMailboxクラスに対するテストを行う為のクラスがActionMailbox::TestCaseです。ActionMailbox::TestCaseでは、受信メールを作成する為のヘルパーメソッドを提供しています。

//list[mailbox_test][ActionMailbox::TestCase]{
class InboxMailboxTest < ActionMailbox::TestCase
  test "receive mail" do
    receive_inbound_email_from_mail \
      to: '"someone" <someone@example.com>',
      from: '"else" <else@example.com>',
      subject: "Hello world!",
      body: "Hello?"

    mail = ReceiveMail.last
    assert_equal "else@example.com", mail.from
    assert_equal "someone@example.com", mail.to
    assert_equal "Hello world!", mail.subject
    assert_equal "Hello?", mail.body
  end
end
//}

他にもソースやfixtureから受信メールを作成する為のヘルパーメソッドが提供されています。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionMailer/TestCase.html}をご参照下さい。

== Rails::Generators::TestCase

Railsで提供しているgenerator(ファイルを生成する為の仕組み)は、カスタマイズ可能で、ユーザが任意のgeneratorを追加出来るようになっています。Rails::Generators::TestCaseはそのgeneratorのテストの為のクラスで、ユーザが追加したgeneratorに対してテストを行えるようになっています。

例えば、"app/forms"配下にファイルを生成するgeneratorがあるとします。

//list[generator][form generator]{
class FormGenerator < Rails::Generators::NamedBase
  source_root File.expand_path('templates', __dir__)

  def create_form_file
    template "form.rb",
      File.join("app/forms", class_path, "#{file_name}_form.rb")
  end
end
//}

このgeneratorに対して、次のようにテストを行う事が出来るようになっています。

//list[generator_test][form generator test]{
class FormGeneratorTest < Rails::Generators::TestCase
  tests FormGenerator
  destination Rails.root.join('tmp/generators')
  setup :prepare_destination

  test "generator runs" do
    assert_nothing_raised do
      run_generator ["user"]
    end

    assert_file "app/forms/user_form.rb"
  end
end
//}

なお、テストを実行すると実際にgeneratorを実行しファイルの生成を行います。ファイルの生成先は@<code>{destination}で指定出来ます。tmpのような、一時ファイルが生成されても良いディレクトリ以外は指定しないよう注意してください。

クラスについてのより詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/Rails/Generators/TestCase.html}をご参照下さい。
